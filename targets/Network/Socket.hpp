/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <array>
#include <netinet/in.h>
#include <netdb.h>

namespace network {

class Socket {
public:
    struct Address {
        sockaddr_storage internal = {};
        socklen_t length = sizeof(sockaddr_storage);

        std::pair<std::string, std::string> getNames() const;
        unsigned short getPort() const;
    };

    struct Details {
        int family;
        int type;
        int protocol;
    };

    static constexpr Details TCP{AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP};
    static constexpr Details TCPv4{AF_INET, SOCK_STREAM, IPPROTO_TCP};
    static constexpr Details TCPv6{AF_INET6, SOCK_STREAM, IPPROTO_TCP};

    Socket() = default;
    void bind(addrinfo* addressInfo);
    void bind(const std::string& name, const std::string& port, const Details& details = TCP);
    void listen(int limit);
    void connect(const std::string& name, const std::string& port, const Details& details = TCP);
    Socket accept();
    Socket accept(Address& address);
    void send(const std::string& message, int flags = 0);
    std::string recv(int flags = 0);
    void close();
    bool isClosed() const;
    Address getAddress() const;
    operator int () const;

private:
    int descriptor = -1;
};

}
