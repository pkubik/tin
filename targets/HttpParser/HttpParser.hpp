/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include "HttpLexer.hpp"

#include <string>
#include <unordered_map>

namespace parser {
namespace http {

class Request {
public:
    enum Method {
        GET,
        POST,
        HEAD
    };

    typedef std::unordered_map<std::string, std::string> HeaderMap;

    Request() = default;
    Request(const Request&) = delete;
    Request& operator =(const Request&) = delete;
    Request(Request&&) = default;

    Method getMethod() const { return method; }
    const std::string& getUri() const { return uri; }
    const std::string& getVersion() const { return version; }
    const HeaderMap& getHeaders() const { return headers; }

private:
    Method method;
    std::string uri;
    std::string version;

    HeaderMap headers;
    // message body is left in the buffer for another parser
};

class Parser {
public:
    Parser(BufferedInput& input) : lexer(input) {}

private:
    Lexer lexer;
};


}
}
