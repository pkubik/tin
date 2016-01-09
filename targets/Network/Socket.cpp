/*
 * TIN 2015
 *
 * Pawel Kubik
 */

#include "Exception.hpp"
#include "Socket.hpp"
#include "Common/Logger.hpp"
#include "Common/Utils.hpp"

#include "utils/fd-utils.hpp"

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <chrono>
#include <thread>

using namespace utils;

namespace network {

namespace {

const int MAX_QUEUE_LENGTH = 1000;
const int RETRY_CONNECT_STEP_MS = 10;
const int UNIX_SOCKET_PROTOCOL = 0;

std::unique_ptr<::addrinfo, void(*)(::addrinfo*)> getAddressInfo(const std::string& host,
                                                                 const std::string& port)
{
    ::addrinfo* addressInfo;

    const char* chost = host.empty() ? nullptr : host.c_str();
    const char* cport = port.empty() ? nullptr : port.c_str();

    int ret = ::getaddrinfo(chost, cport, nullptr, &addressInfo);
    if (ret != 0) {
        const std::string msg = "Failed to get address info: " + std::string(::gai_strerror(ret));
        LOGE(msg);
        throw NetworkException(msg);
    }

    return std::unique_ptr<::addrinfo, void(*)(::addrinfo*)>(addressInfo, ::freeaddrinfo);
}

void connect(const int socket,
             const ::sockaddr* address,
             const ::socklen_t addressLength,
             const unsigned int timeoutMS)
{
    auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(timeoutMS);

    do {
        if (-1 != ::connect(socket,
                            address,
                            addressLength)) {
            return;
        }

        if (errno == ECONNREFUSED || errno == EAGAIN || errno == EINTR) {
            // No one is listening, so sleep and retry
            LOGW("No one listening on the socket, retrying");
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_CONNECT_STEP_MS));
            continue;
        }

        // Error
        utils::close(socket);
        const std::string msg = "Error in connect: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);

    } while (std::chrono::steady_clock::now() < deadline);

    const std::string msg = "Timeout in connect";
    LOGE(msg);
    throw NetworkException(msg);
}

int getSocketFd(const int family, const int type, const int protocol)
{
    int fd = ::socket(family, type, protocol);
    if (fd == -1) {
        const std::string msg = "Error in socket: " + getErrorMessage();
        LOGE(msg);
        throw SocketException(msg, errno);
    }

    return fd;
}

int getConnectedFd(const int family,
                   const int type,
                   const int protocol,
                   const ::sockaddr* address,
                   const ::socklen_t addressLength,
                   const int timeoutMs)
{
    int fd = getSocketFd(family, type, protocol);
    connect(fd, address, addressLength, timeoutMs);

    return fd;
}

int getBoundFd(const int family,
               const int type,
               const int protocol,
               const ::sockaddr* address,
               const ::socklen_t addressLength)
{
    int fd = getSocketFd(family, type, protocol);

    // Ensure address doesn't exist before bind() to avoid errors
    ::unlink(reinterpret_cast<const ::sockaddr_un*>(address)->sun_path);

    if (-1 == ::bind(fd, address, addressLength)) {
        utils::close(fd);
        const std::string msg = "Error in bind: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    if (-1 == ::listen(fd,
                       MAX_QUEUE_LENGTH)) {
        utils::close(fd);
        const std::string msg = "Error in listen: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return fd;
}

} // namespace

Socket::Socket(int socketFD)
    : mFD(socketFD)
{
}

Socket::Socket(Socket&& socket) noexcept
    : mFD(socket.mFD)
{
    socket.mFD = -1;
}

Socket& Socket::operator=(Socket&& socket)
{
    close();
    mFD = socket.mFD;
    socket.mFD = -1;

    return *this;
}

Socket::~Socket() noexcept
{
    close();
}

int Socket::getFD() const
{
    return mFD;
}

Socket Socket::accept()
{
    int sockfd = ::accept(mFD, nullptr, nullptr);
    if (sockfd == -1) {
        const std::string msg = "Error in accept: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }
    return Socket(sockfd);
}

void Socket::close()
{
    try {
        utils::close(mFD);
    } catch (std::exception& e) {
        LOGE("Error in Socket's destructor: " << e.what());
    }

    mFD = -1;
}

Socket::Type Socket::getType() const
{
    int family;
    socklen_t length = sizeof(family);

    if (::getsockopt(mFD, SOL_SOCKET, SO_DOMAIN, &family, &length)) {
        if (errno == EBADF) {
            return Type::INVALID;
        } else {
            const std::string msg = "Error getting socket type: " + getErrorMessage();
            LOGE(msg);
            throw NetworkException(msg);
        }
    }


    if (family == AF_UNIX || family == AF_LOCAL) {
        return Type::UNIX;
    }

    if (family == AF_INET || family == AF_INET6) {
        return Type::INET;
    }

    return Type::INVALID;
}

unsigned short Socket::getPort() const
{
    ::sockaddr_storage address = {0, 0, {0}};
    ::socklen_t length = sizeof(address);
    if (::getsockname(mFD, reinterpret_cast<sockaddr*>(&address), &length) != 0) {
        const std::string msg = "Failed to get socked address: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    if (length == sizeof(sockaddr_in)) {
        return ntohs(reinterpret_cast<const sockaddr_in*>(&address)->sin_port);
    } else {
        return ntohs(reinterpret_cast<const sockaddr_in6*>(&address)->sin6_port);
    }
}

void Socket::write(const void* bufferPtr, const size_t size) const
{
    utils::write(mFD, bufferPtr, size);
}

void Socket::read(void* bufferPtr, const size_t size) const
{
    utils::read(mFD, bufferPtr, size);
}

size_t Socket::send(const void* bufferPtr, const size_t size) const
{
    int ret = ::send(mFD, bufferPtr, size, 0);
    if (ret == -1) {
        const std::string msg = "Socket send error: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return ret;
}

size_t Socket::receive(void* bufferPtr, const size_t size) const
{
    int ret = ::recv(mFD, bufferPtr, size, 0);
    if (ret == -1) {
        const std::string msg = "Socket receive error: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return ret;
}

int Socket::createSocketInternal(const std::string& path)
{
    // Isn't the path too long?
    if (path.size() >= sizeof(sockaddr_un::sun_path)) {
        const std::string msg = "Socket's path too long";
        LOGE(msg);
        throw NetworkException(msg);
    }

    ::sockaddr_un serverAddress;
    serverAddress.sun_family = AF_UNIX;
    ::strncpy(serverAddress.sun_path, path.c_str(), path.length() + 1);

    return getBoundFd(AF_UNIX,
                      SOCK_STREAM,
                      UNIX_SOCKET_PROTOCOL,
                      reinterpret_cast<struct sockaddr*>(&serverAddress),
                      sizeof(struct sockaddr_un));
}

Socket Socket::createUNIX(const std::string& path)
{
    // Initialize a socket
    int fd;
    fd = createSocketInternal(path);

    return Socket(fd);
}

Socket Socket::createINET(const std::string& host, const std::string& service)
{
    auto address = getAddressInfo(host, service);

    int fd = getBoundFd(address->ai_family,
                        address->ai_socktype,
                        address->ai_protocol,
                        address->ai_addr,
                        address->ai_addrlen);

    return Socket(fd);
}

Socket Socket::connectUNIX(const std::string& path, const int timeoutMs)
{
    // Isn't the path too long?
    if (path.size() >= sizeof(::sockaddr_un::sun_path)) {
        const std::string msg = "Socket's path too long";
        LOGE(msg);
        throw NetworkException(msg);
    }

    // Fill address
    struct ::sockaddr_un serverAddress;
    serverAddress.sun_family = AF_UNIX;
    ::strncpy(serverAddress.sun_path, path.c_str(), sizeof(::sockaddr_un::sun_path));

    int fd = getConnectedFd(AF_UNIX,
                            SOCK_STREAM,
                            UNIX_SOCKET_PROTOCOL,
                            reinterpret_cast<struct sockaddr*>(&serverAddress),
                            sizeof(struct ::sockaddr_un),
                            timeoutMs);

    return Socket(fd);
}

Socket Socket::connectINET(const std::string& host, const std::string& service, const int timeoutMs)
{
    auto addressInfo = getAddressInfo(host, service);

    int fd = getConnectedFd(addressInfo->ai_family,
                            addressInfo->ai_socktype,
                            addressInfo->ai_protocol,
                            reinterpret_cast<::sockaddr*>(addressInfo->ai_addr),
                            addressInfo->ai_addrlen,
                            timeoutMs);

    return Socket(fd);
}

}
