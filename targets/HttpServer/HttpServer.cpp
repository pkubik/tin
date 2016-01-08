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

void Server::start() {
    Socket socket = Socket::createINET("localhost", std::to_string(port));

    for (;;) {
        pool.execute(socket.accept());
    }
}

}
