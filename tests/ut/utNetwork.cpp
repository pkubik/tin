/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include <thread>

#include "Network/Socket.hpp"
#include "Network/Exception.hpp"

namespace net = Network;

TEST_CASE( "Socket init and close", "[Network::Socket]" ) {
    net::Socket socket;
    REQUIRE(socket.isClosed());

    socket.bind("localhost", "0", net::Socket::TCP);
    REQUIRE(!socket.isClosed());

    socket.close();
    REQUIRE(socket.isClosed());
}

TEST_CASE( "Unknown address", "[Network::Socket]" ) {
    net::Socket socket;

    REQUIRE_THROWS(socket.bind(".*/definitely_unknown=address", "and0port", net::Socket::TCP));

    REQUIRE(socket.isClosed());
}

TEST_CASE( "Socket listen error", "[Network::Socket]" ) {
    net::Socket socket;

    REQUIRE_THROWS(socket.listen(0));
}

TEST_CASE( "Socket send and recv", "[Network::Socket]" ) {
    net::Socket server;
    server.bind("localhost", "0", net::Socket::TCPv4);
    auto address = server.getAddress();
    server.listen(10);

    auto clientThread = std::thread([&] {
        net::Socket client;
        client.connect("localhost", std::to_string(address.getPort()), net::Socket::TCP);
        client.send("ABC");
        REQUIRE(client.recv() == "XYZ");
    });

    net::Socket::Address addr;
    auto connection = server.accept(addr);
    REQUIRE(connection.recv() == "ABC");
    connection.send("XYZ");

    clientThread.join();
}
