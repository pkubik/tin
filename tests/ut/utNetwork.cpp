/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include "Network/Socket.hpp"
#include "Network/Exception.hpp"

#include <thread>

using namespace network;

namespace {

const std::string SOCKET_PATH = "/tmp/test.socket";

}

TEST_CASE( "Get socket type", "[Network::Socket]" ) {
    {
        Socket socket;
        CHECK(socket.getType() == Socket::Type::INVALID);
    }

    {
        Socket socket = Socket::createINET("localhost", "");
        CHECK(socket.getType() == Socket::Type::INET);
    }
}

TEST_CASE( "Internet socket communication", "[Network::Socket]" ) {
    const char msg[] = "MESSAGE";
    const std::string host = "127.0.0.1";

    Socket server = Socket::createINET(host, "");
    const unsigned short port = server.getPort();

    CHECK(server.getType() == Socket::Type::INET);

    auto clientThread = std::thread([&] {
        Socket client = Socket::connectINET(host, std::to_string(port));
        CHECK(client.getType() == Socket::Type::INET);
        CHECK(client.send(msg, sizeof(msg)) == sizeof(msg));

        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        char buffer[sizeof(msg) + 1];
        CHECK(client.receive(buffer, sizeof(msg)) == sizeof(msg));
        CHECK(std::string(buffer) == std::string(msg));
    });

    auto connection = server.accept();
    char buffer[sizeof(msg)];

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    CHECK(connection.receive(buffer, sizeof(msg)) == sizeof(msg));
    CHECK(std::string(buffer) == std::string(msg));

    CHECK(connection.send(msg, sizeof(msg)) == sizeof(msg));

    clientThread.join();
}
