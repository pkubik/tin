/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "Utils.hpp"

const std::string getErrorMessage() {
    return std::string(std::strerror(errno));
}
