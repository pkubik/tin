/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpParser.hpp"
#include "HttpLexer.hpp"

#include <iostream>

namespace parser {
namespace http {

Parser::Result Parser::parse(BufferedInput& input) {
    Result result;

    Parser parser(input, result.first, result.second);

    parser.parse();

    return result;
}

void Parser::parse() {
    if (!parseMethod()) {
        return;
    }

    if (!parseWhitespace()) {
        return;
    }

    if (token.type == Token::WORD) {
        request.uri = token.value;
        token = lexer.getToken();
    } else {
        status.setError("Error parsing URI");
        return;
    }

    if (!parseWhitespace()) {
        return;
    }

    if (token.type == Token::WORD) {
        request.version = token.value;
        token = lexer.getToken();
    } else {
        status.setError("Error parsing HTTP version");
        return;
    }

    skipBlanks();

    if (!parseCRLF()) {
        return;
    }

    // TODO: Parse the rest of the header
}

bool Parser::parseMethod() {
    bool ret = true;

    if (token.type == Token::WORD) {
        if (token.value == "GET") {
            request.method = Request::GET;
        } else if (token.value == "POST") {
            request.method = Request::POST;
        } else if (token.value == "HEAD") {
            request.method = Request::HEAD;
        } else {
            status.setError("Unknown method type");
            ret = false;
        }
    } else {
        status.setError("Unknown method type");
        ret = false;
    }

    token = lexer.getToken();
    return ret;
}

bool Parser::parseWhitespace() {
    bool ret = false;

    while (token.type == Token::BLANK || token.type == Token::CRLF) {
        ret = true;
        token = lexer.getToken();
    }

    if (!ret) {
        status.setError("Whitespace expected");
    }

    return ret;
}

void Parser::skipBlanks() {
    while (token.type == Token::BLANK) {
        token = lexer.getToken();
    }
}

bool Parser::parseCRLF() {
    bool ret = true;

    if (token.type != Token::CRLF) {
        status.setError("CRLF expected");
        ret = false;
    }

    token = lexer.getToken();
    return ret;
}

}
}
