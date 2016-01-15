/*
 * TIN 2015
 *
 * Pawel Kubik
 */

#pragma once

#include <string>
#include <netdb.h>

namespace network {

/**
 * This class wraps all operations possible to do with a socket.
 *
 * It has operations both for client and server application.
 */
class Socket {
public:
    enum class Type : int8_t {
        INVALID,
        UNIX,
        INET
    };

    /**
     * Default constructor.
     * If socketFD is passed then it's passed by the Socket
     *
     * @param socketFD socket obtained outside the class.
     */
    explicit Socket(int socketFD = -1);
    Socket(Socket&& socket) noexcept;
    Socket& operator=(Socket&& socket);
    ~Socket() noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    /**
     * @return reference to the socket's file descriptor
     */
    int getFD() const;

    /**
     * Send data using the file descriptor
     *
     * @param bufferPtr buffer with the data
     * @param size size of the buffer
     */
    size_t send(const void* bufferPtr, const size_t size) const;

    /**
     * Receive data from the file descriptor.
     *
     * @param bufferPtr buffer with the data
     * @param size size of the buffer
     */
    size_t receive(void* bufferPtr, const size_t size) const;

    /**
     * Accepts connection. Used by a server application.
     * Blocking, called by a server.
     */
    Socket accept();

    /**
     * Closes socket descriptor.
     */
    void close();

    /**
     * Returns the socket type based on it's domain.
     */
    Type getType() const;

    /**
     * Returns a port associated with the socket.
     */
    unsigned short getPort() const;

    /**
     * Prepares UNIX socket for accepting connections.
     * Called by a server.
     *
     * @param path path to the socket
     * @return created socket
     */
    static Socket createUNIX(const std::string& path);

    /**
     * Prepares INET socket for accepting connections.
     * Called by a server.
     *
     * @param host hostname or ip address
     * @param service port number or service name
     * @return created socket
     */
    static Socket createINET(const std::string& host, const std::string& service);

    /**
     * Connects to an UNIX socket. Called as a client.
     *
     * @param path path to the socket
     * @return connected socket
     */
    static Socket connectUNIX(const std::string& path, const int timeoutMs = 1000);

    /**
     * Connects to an INET socket. Called as a client.
     *
     * @param host hostname or ip address
     * @param service port number or service name
     * @return connected socket
     */
    static Socket connectINET(const std::string& host,
                              const std::string& service,
                              const int timeoutMs = 1000);

private:
    int mFD;

    static int createSocketInternal(const std::string& path);
    static int getSystemdSocketInternal(const std::string& path);
};

}
