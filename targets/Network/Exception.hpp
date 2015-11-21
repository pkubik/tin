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

struct NetworkException : public std::runtime_error {
    NetworkException(const std::string& message)
        : std::runtime_error(message) {}
};
