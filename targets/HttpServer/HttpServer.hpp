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
#include "Network/Socket.hpp"

#include <chrono>

namespace server {

class Server {
public:
    static const unsigned short DEFAULT_PORT = 80;
    static const unsigned DEFAULT_DEADLINE = 1000;

    /*
     * @port              port used by server
     * @executorsDeadline time which stopped server waits until closing running handlers in ms
     */
    Server(Handler& handler,
           unsigned short port = DEFAULT_PORT,
           unsigned executorsDeadline = DEFAULT_DEADLINE)
        : handler(handler)
        , port(port)
        , deadline(executorsDeadline)
    {}

    void start();

private:
    Handler& handler;
    short unsigned port = DEFAULT_PORT;
    unsigned deadline = DEFAULT_DEADLINE;

    void handle(network::Socket&& connection);
};

}
