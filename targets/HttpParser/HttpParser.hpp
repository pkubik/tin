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
#include "ParameterLexer.hpp"

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
    typedef std::unordered_map<std::string, std::string> ParameterMap;

    Request() = default;
    Request(const Request&) = delete;
    Request& operator =(const Request&) = delete;
    Request(Request&&) = default;

    Method getMethod() const { return method; }
    const std::string& getResource() const { return resource; }
    const std::string& getVersion() const { return version; }
    const HeaderMap& getHeaders() const { return headers; }
    const ParameterMap& getParameters() const { return parameters; }

private:
    Method method;
    std::string resource;
    std::string version;

    HeaderMap headers;
    ParameterMap parameters;

    friend class Parser;
};

class Parser {
public:
    class Status {
    public:
        bool isValid() const { return !error; }
        operator bool () { return isValid(); }
        const std::string& getErrorMessage() const { return message; }

    private:
        bool error = false;
        std::string message;

        void setError(const std::string& msg) {
            error = true;
            message = msg;
        }

        friend Parser;
    };

    typedef std::pair<Request, Status> Result;

    static Result parse(BufferedInput& input);

private:
    BufferedInput& input;
    Lexer lexer;
    parameter::Lexer parameterLexer;
    std::unique_ptr<Token> token;
    parameter::Token paramToken;
    Request& request;
    Status& status;

    Parser(BufferedInput& input, Request& request, Status& status)
        : input(input)
        , lexer(input)
        , parameterLexer(input)
        , token(lexer.getToken())
        , request(request)
        , status(status) {}

    void parse();

    /*
     * All boolean parse internal methods returns true if succesfully parsed.
     * False doesn't necessarily mean failure!
     */

    /** Returns true if succesfully parsed a Method */
    bool parseMethod();

    /** Returns true if succesfully parsed a header */
    bool parseHeader();

    /** Returns true if succesfully parsed a header value */
    bool parseHeaderValue(std::string& result);

    /** Parses any number of whitespaces */
    bool parseWhitespace();

    /** Skips any number of spaces or tabs */
    void skipBlanks();

    /** Parses single CRLF */
    bool parseCRLF();

    /** Parses uri-encoded parameters */
    bool parseParameters();

    /** Parses one uri-encoded parameter */
    bool parseParameter();
};

}
}
