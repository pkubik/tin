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

#include <list>
#include <thread>
#include <mutex>

namespace server {

class WorkerPool;

struct Worker {
public:
    Worker(WorkerPool& pool);
    void start();
    void work();
    void stop();
    void execute(network::Socket&& connection);
    ~Worker();

private:
    WorkerPool& pool;
    std::thread thread;
    network::Socket connection;
    int pipe[2];

    void handle();
};

/**
 * @brief Manages working threads hanling HTTP requests.
 *
 * This is the HTTP server specific pool. It can't be used to handle arbitrary tasks.
 */
class WorkerPool {
public:
    Handler& handler;

    WorkerPool(Handler& handler) : handler(handler) {}
    void stop();
    void execute(network::Socket&& connection);
    void reuse(Worker* worker);
    ~WorkerPool();

private:
    std::mutex mutex;
    std::list<Worker> workers;
    std::list<Worker*> readyWorkers;
};

}
