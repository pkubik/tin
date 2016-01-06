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

class Handler {
public:
    virtual Response handle(const Request&) = 0;
    virtual Response handleError() = 0;
};

}
