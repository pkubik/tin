/*
 * TIN 2015
 *
 * Pawel Kubik
 */

#include "Abortable.hpp"

#include <sys/poll.h>

namespace network {

std::pair<size_t, bool> abortableRead(Socket& socket,
                                      char* buffer,
                                      const size_t size,
                                      const short pipeEnd,
                                      const int timeout)
{
    ::pollfd fds[] = {{socket.getFD(), (POLLIN|POLLERR|POLLPRI), 0},
                      {pipeEnd, (POLLIN|POLLERR|POLLPRI), 0}};

    size_t ready = 0;
    do {
        int ret = ::poll(fds, 2, timeout);

        if (!(ret == 1 && fds[0].revents & POLLIN)) {
            return std::make_pair(ready, true);
        }

        ready += socket.receive(buffer + ready, size - ready);
    } while (ready == 0);

    return std::make_pair(ready, false);
}

bool abortableWriteAll(Socket& socket,
                       const char* buffer,
                       const size_t size,
                       const short pipeEnd,
                       const int timeout)
{
    ::pollfd fds[] = {{socket.getFD(), (POLLOUT|POLLERR|POLLPRI), 0},
                      {pipeEnd, (POLLIN|POLLERR|POLLPRI), 0}};

    size_t sent = 0;
    do {
        int ret = ::poll(fds, 2, timeout);

        if (!(ret == 1 && fds[0].revents & POLLOUT)) {
            return true;
        }

        sent += socket.send(buffer + sent, size - sent);
    } while (sent < size);

    return false;
}

bool abortableReadAll(Socket& socket,
                      char* buffer,
                      const size_t size,
                      const short pipeEnd,
                      const int timeout)
{
    ::pollfd fds[] = {{socket.getFD(), (POLLIN|POLLERR|POLLPRI), 0},
                      {pipeEnd, (POLLIN|POLLERR|POLLPRI), 0}};

    size_t ready = 0;
    do {
        int ret = ::poll(fds, 2, timeout);

        if (!(ret == 1 && fds[0].revents & POLLIN)) {
            return true;
        }

        ready += socket.receive(buffer + ready, size - ready);
    } while (ready < size);

    return false;
}

bool abortableWaitForConnection(Socket& socket, const int pipeEnd, const unsigned timeout) {
    ::pollfd pfds[2] = {{socket.getFD(), POLLIN|POLLERR|POLLPRI, 0},
                        {pipeEnd, POLLIN, 0}};

    do {
        int ret = ::poll(pfds, 2, timeout);

        if (ret == 1 && pfds[0].revents & POLLIN) {
            return false;
        }

    } while (pfds[1].revents & EINTR);

    return true;
}

}
