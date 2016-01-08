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

namespace server {

struct ServerException : public std::runtime_error {
    ServerException(const std::string& message)
        : std::runtime_error(message) {}
};

}
