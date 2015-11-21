/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include "Common/Logger.hpp"
#include "Common/Utils.hpp"

#include "Socket.hpp"
#include "Exception.hpp"

namespace Network {

constexpr Socket::Details Socket::TCP;
constexpr Socket::Details Socket::TCPv4;
constexpr Socket::Details Socket::TCPv6;

namespace {

int createSocket(int family, int type, int protocol) {
    int descriptor = ::socket(family, type, protocol);

    if (descriptor < 0) {
        const std::string msg = "Failed to create socket: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return descriptor;
}

}

void Socket::bind(addrinfo* addressInfo) {
    const auto& address = addressInfo->ai_addr;
    const auto& length = addressInfo->ai_addrlen;
    if (::bind(descriptor, address, length) < 0) {
        close();
        const std::string msg = "Failed to bind socket: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }
}

void Socket::bind(const std::string& name,
                  const std::string& port,
                  const Socket::Details& details) {
    addrinfo hints = {};
    hints.ai_family = details.family;
    hints.ai_socktype = details.type;

    addrinfo* aiPtr = nullptr;

    int rcode = ::getaddrinfo(name.c_str(), port.c_str(), &hints, &aiPtr);
    if (rcode != 0) {
        const std::string msg = "Failed to get address info: " + std::string(gai_strerror(rcode));
        LOGI(msg);
        throw NetworkException(msg);
    }

    close();
    descriptor = createSocket(aiPtr->ai_family, aiPtr->ai_socktype, aiPtr->ai_protocol);
    bind(aiPtr);
}

void Socket::listen(int limit) {
    if (::listen(descriptor, limit) < 0) {
        close();
        const std::string msg = "Failed listen on socket: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }
}

void Socket::connect(const std::string& name, const std::string& port, const Socket::Details& details) {
    addrinfo hints = {};
    hints.ai_family = details.family;
    hints.ai_socktype = details.type;
    hints.ai_protocol = details.protocol;

    addrinfo* aiPtr = nullptr;

    int rcode = ::getaddrinfo(name.c_str(), port.c_str(), &hints, &aiPtr);
    if (rcode != 0) {
        const std::string msg = "Failed to get address info: " + std::string(gai_strerror(rcode));
        LOGI(msg);
        throw NetworkException(msg);
    }

    close();
    descriptor = createSocket(aiPtr->ai_family, aiPtr->ai_socktype, aiPtr->ai_protocol);

    const auto& address = aiPtr->ai_addr;
    const auto& length = aiPtr->ai_addrlen;
    if (::connect(descriptor, address, length) == -1) {
        close();
        const std::string msg = "Failed to bind socket: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }
}

Socket Socket::accept() {
    Socket socket;
    socket.descriptor = ::accept(descriptor, nullptr, nullptr);

    if (socket.descriptor < 0) {
        const std::string msg = "Failed to accept pending connection: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return socket;
}

Socket Socket::accept(Socket::Address& address) {
    Socket socket;
    socket.descriptor = ::accept(descriptor,
                                 reinterpret_cast<sockaddr*>(&address.internal),
                                 &address.length);

    if (socket.descriptor < 0) {
        const std::string msg = "Failed to accept pending connection: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return socket;
}

void Socket::send(const std::string& message, int flags) {
    size_t length = message.size();
    const char* raw = message.c_str();

    do {
        int sent = ::send(descriptor, raw, length, flags);
        raw += sent;
        length -= sent;
    } while (length > 0);
}

std::string Socket::recv(int flags) {
    const int length = 1024;
    std::string ret;
    int received;
    char buffer[1024];

    do {
        received = ::recv(descriptor, buffer, length, flags);

        if (received < 0) {
            const std::string msg = "Failed to receive a message: " + getErrorMessage();
            LOGE(msg);
            throw NetworkException(msg);
        }

        buffer[received] = '\0';
        ret += buffer;
    } while (received >= length);

    return ret;
}

void Socket::close() {
    if (descriptor >= 0) {
        ::close(descriptor);
        descriptor = -1;
    }
}

bool Socket::isClosed() const {
    return descriptor < 0;
}

Socket::Address Socket::getAddress() const {
    Address address;

    sockaddr* addr = reinterpret_cast<sockaddr*>(&address.internal);
    if (::getsockname(descriptor, addr, &address.length) != 0) {
        const std::string msg = "Failed to get socked address: " + getErrorMessage();
        LOGE(msg);
        throw NetworkException(msg);
    }

    return address;
}

Socket::operator int () const {
    return descriptor;
}

std::pair<std::string, std::string> Socket::Address::getNames() const {
    char hostBuffer[NI_MAXHOST];
    char servBuffer[NI_MAXSERV];
    hostBuffer[0] = '\0';
    servBuffer[0] = '\0';
    ::getnameinfo((sockaddr*)(&internal), length, hostBuffer, NI_MAXHOST, servBuffer, NI_MAXSERV, 0);

    return std::make_pair(std::string(hostBuffer, NI_MAXHOST), std::string(servBuffer, NI_MAXSERV));
}

unsigned short Socket::Address::getPort() const {
    if (length == sizeof(sockaddr_in)) {
        return ::ntohs(reinterpret_cast<const sockaddr_in*>(&internal)->sin_port);
    } else {
        return ::ntohs(reinterpret_cast<const sockaddr_in6*>(&internal)->sin6_port);
    }
}

}
