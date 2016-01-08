/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include "HttpParser/HttpParser.hpp"
#include "HttpParser/HttpGenerator.hpp"

namespace server {

typedef parser::http::Request Request;
typedef parser::http::Response Response;

enum class RequestError {
    NONE,
    PARSE,
    TIMEOUT
};

class Handler {
public:
    virtual Response handle(const Request&, RequestError = RequestError::NONE) = 0;
};

}
