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
#include "Network/Abortable.hpp"
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

}

Server::Server(Handler& handler, unsigned short port)
    : pool(handler)
    , port(port)
{
    registerSignalHandlers();
    if (::pipe(pipe) == -1) {
        throw ServerException("Failed to create server's pipe.");
    }
    socket = Socket::createINET("0.0.0.0", std::to_string(port));
    this->port = socket.getPort();
}

void Server::start() {
    while (!abortableWaitForConnection(socket, pipe[0])) {
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
