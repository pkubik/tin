/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "WorkerPool.hpp"
#include "Handler.hpp"
#include "Common/Logger.hpp"
#include "Exception.hpp"

#include <sys/poll.h>
#include <unistd.h>

using namespace parser;
using namespace network;

namespace server {

namespace {

class SourceWrapper : public SourceReader {
private:
    Socket& socket;
    ::pollfd fds[2];

public:
    SourceWrapper(Socket& socket, int pipeEnd)
        : socket(socket)
        , fds{{socket.getFD(), POLLIN, 0},
              {pipeEnd, POLLIN, 0}}
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

bool waitForWork(int pipeEnd, unsigned timeout = -1) {
    ::pollfd pfd = {pipeEnd, POLLIN, 0};
    int ret = ::poll(&pfd, 1, timeout);
    if (ret == 1 && pfd.revents & POLLIN) {
        char buffer;
        if (::read(pipeEnd, &buffer, 1) == 1 && buffer == '1') {
            LOGD("Worker received a new task.");
            return true;
        }
    }

    return false;
}

}

Worker::Worker(WorkerPool &pool)
    : pool(pool)
{
    if (::pipe(pipe) == -1) {
        throw ServerException("Failed to create worker's pipe.");
    }
}

void Worker::start() {
    thread = std::thread([this]() {
        work();
    });
}

void Worker::work() {
    while (waitForWork(pipe[0])) {
        handle();
        pool.reuse(this);
    }
}

void Worker::stop() {
    ::write(pipe[1], "0", 1);
}

void Worker::execute(Socket&& connection) {
    this->connection = std::move(connection);
    ::write(pipe[1], "1", 1);
}

void Worker::handle() {
    SourceWrapper source(connection, pipe[0]);
    BufferedInput input(source, 100);
    auto result = http::Parser::parse(input);
    if (!result.second) {
        LOGW("Received invalid request.");
        const auto& response = pool.handler.handle(result.first, RequestError::PARSE);
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    } else {
        LOGT("Handling request...");
        const auto& response = pool.handler.handle(result.first, RequestError::NONE);
        const auto& responseText = http::Generator::generate(response);
        connection.write(responseText.c_str(), responseText.length());
    }

    connection.close();
}

Worker::~Worker() {
    stop();
    LOGD("Waiting for a worker thread to exit...");
    thread.join();
    LOGD("Worker thread exited.");
    ::close(pipe[0]);
    ::close(pipe[1]);
}

void WorkerPool::stop() {
    for (Worker& worker : workers) {
        worker.stop();
    }

    workers.clear();
}

void WorkerPool::reuse(Worker* worker) {
    std::lock_guard<std::mutex> lock(mutex);

    readyWorkers.push_back(worker);
}

WorkerPool::~WorkerPool() {
    stop();
}

void WorkerPool::execute(Socket&& connection) {
    std::lock_guard<std::mutex> lock(mutex);
    LOGT("Executing task (" << readyWorkers.size() << "/" << workers.size() << " workers ready).");

    Worker* worker;
    if (readyWorkers.empty()) {
        LOGT("Creating new worker.");
        workers.emplace_back(*this);
        worker = &workers.back();
        worker->start();
    } else {
        worker = readyWorkers.front();
        readyWorkers.pop_front();
    }

    worker->execute(std::move(connection));
}

}
