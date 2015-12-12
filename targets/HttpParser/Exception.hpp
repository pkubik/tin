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

struct ParserException : public std::runtime_error {
    ParserException(const std::string& message)
        : std::runtime_error(message) {}
};
