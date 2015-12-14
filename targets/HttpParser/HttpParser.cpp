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

namespace {

bool isText(const Token& token) {
    return token.type != Token::CRLF && token.type != Token::END;
}

}

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

    while (parseHeader());

    parseCRLF();
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

bool Parser::parseHeader() {
    if (token.type != Token::WORD) {
        if (token.type == Token::CRLF) {
            // header list is empty
            return false;
        }
        status.setError("Error parsing header name");
        return false;
    }
    auto it = request.headers.insert(std::make_pair(token.value, ""));
    // NOTE: according to RFC2616 parser should know all possible header-fields and
    // their value structures and decide whether it is correct to allow header repetition

    token = lexer.getToken();

    skipBlanks();

    if (token.type != Token::COLON) {
        status.setError("Colon between header name and value expected");
        return false;
    }
    token = lexer.getToken();

    skipBlanks();

    std::string value;
    if (!parseHeaderValue(value)) {
        return false;
    }

    it.first->second = value;

    return true;
}

bool Parser::parseHeaderValue(std::string& result) {
    for (;;) {
        while (isText(token)) {
            result += token.value;
            token = lexer.getToken();
        }

        if (token.type == Token::CRLF) {
            token = lexer.getToken();
            if (token.type == Token::BLANK) {
                skipBlanks();
                result += ' ';
                continue;
            } else {
                return true;
            }
        } else {
            status.setError("Error parsing header value.");
            return false;
        }
    }

    return true;
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
