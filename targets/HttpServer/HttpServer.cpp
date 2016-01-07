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

using namespace network;
using namespace parser;

namespace server {

namespace {

class SourceWrapper : public SourceReader {
private:
    Socket& socket;

public:
    SourceWrapper(Socket& socket) : socket(socket) {}

    virtual int read(char* buffer, size_t length) {
        return socket.receive(buffer, length);
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

    SourceWrapper source(connection);
    BufferedInput input(source, 100);
    auto result = http::Parser::parse(input);
    if (!result.second) {
        LOGW("Received invalid request.");
        const auto& response = handler.handleError();
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    } else {
        LOGT("Handling request...");
        const auto& response = handler.handle(result.first);
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    }
}

}
