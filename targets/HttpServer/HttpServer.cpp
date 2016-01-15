/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpServer.hpp"
#include "Exception.hpp"
#include "Common/Logger.hpp"

#include <signal.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace network;
using namespace parser;

namespace server {

namespace {

static Server* serverInstance;

void handleSignal(int signo) {
    if (signo == SIGINT) {
        LOGI("Received SIGINT. Closing worker threads...");
        serverInstance->stop();
        LOGI("Server closed.");
    }
}

bool waitForConnection(Socket& socket, int pipeEnd, unsigned timeout = -1) {
    ::pollfd pfds[2] = {{socket.getFD(), POLLIN, 0},
                        {pipeEnd, POLLIN, 0}};

    int ret = ::poll(pfds, 2, timeout);

    if (ret == 1 && pfds[0].revents & POLLIN) {
        return true;
    }

    return false;
}

}

Server::Server(Handler& handler, unsigned short port)
    : pool(handler)
    , port(port)
{
    registerSignalHandlers();
    if (::pipe(pipe) == -1) {
        throw ServerException("Failed to create server's pipe.");
    }
    socket = Socket::createINET("", std::to_string(port));
    this->port = socket.getPort();
}

void Server::start() {
    while (waitForConnection(socket, pipe[0])) {
        pool.execute(socket.accept());
    }

    LOGT("Closed server main loop.");
    LOGT("Closing worker pool...");
    pool.stop();
}

void Server::stop() {
    ::write(pipe[1], "0", 1);
}

void Server::registerSignalHandlers() {
    serverInstance = this;
    if (signal(SIGINT, handleSignal) == SIG_ERR) {
        const std::string msg = "Inable to set SIGINT signal handler";
        LOGE(msg);
        throw ServerException(msg);
    }
}

}
