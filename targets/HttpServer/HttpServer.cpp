/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpServer.hpp"
#include "Common/Logger.hpp"

#include <sys/poll.h>

using namespace network;
using namespace parser;

namespace server {

namespace {

class SourceWrapper : public SourceReader {
private:
    Socket& socket;
    ::pollfd fds[2];

public:
    SourceWrapper(Socket& socket, int pipeEnd)
        : socket(socket)
        , fds({{socket.getFD(), POLLIN, 0},
               {pipeEnd, POLLIN, 0}})
    {}

    virtual int read(char* buffer, size_t length) {
        int ret = ::poll(fds, 2, 2000);

        if (ret == 1 && fds[0].revents & POLLIN) {
            return socket.receive(buffer, length);
        } else {
            return 0;
        }
    }
};

}

void Server::start() {
    Socket socket = Socket::createINET("localhost", std::to_string(port));

    for (;;) {
        handle(socket.accept());
    }
}

void Server::handle(Socket&& connection) {
    // this function should be called from worker thread

    SourceWrapper source(connection, 0); // TODO: input executors pipe end instead of 0
    BufferedInput input(source, 100);
    auto result = http::Parser::parse(input);
    if (!result.second) {
        LOGW("Received invalid request.");
        const auto& response = handler.handle(result.first, RequestError::PARSE);
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    } else {
        LOGT("Handling request...");
        const auto& response = handler.handle(result.first, RequestError::NONE);
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    }
}

}
