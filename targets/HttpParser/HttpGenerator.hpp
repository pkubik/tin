/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <cinttypes>
#include <string>
#include <map>

namespace parser {
namespace http {

struct Response {
    uint16_t code;
    std::map<std::string, std::string> headers;
    std::string message;

    // ensure it is not copied
    Response() = default;
    Response(Response&&) = default;
};

}
}
