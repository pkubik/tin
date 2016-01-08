/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include "Handler.hpp"
#include "WorkerPool.hpp"
#include "Network/Socket.hpp"

#include <chrono>

namespace server {

class Server {
public:
    static const unsigned short DEFAULT_PORT = 80;

    /*
     * @port              port used by server
     */
    Server(Handler& handler, unsigned short port = DEFAULT_PORT)
        : pool(handler)
        , port(port)
    {}

    void start();

private:
    WorkerPool pool;
    short unsigned port = DEFAULT_PORT;
};

}
