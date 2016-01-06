/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <string>
#include <stdexcept>

namespace network {

struct NetworkException : public std::runtime_error {
    NetworkException(const std::string& message)
        : std::runtime_error(message) {}
};

struct SocketException : public NetworkException {
    SocketException(const std::string& message, const int code)
        : NetworkException(message), code(code) {}

    int getCode() const { return code; }

private:
    int code;
};

}
