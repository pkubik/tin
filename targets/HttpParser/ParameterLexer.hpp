/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include "BufferedInput.hpp"

#include <string>

namespace parser {
namespace parameter {

struct Token {
    enum Type {
        TEXT,
        EQUAL,
        AND,
        ERROR,
        END
    };

    Type type;
    std::string value;
};

/**
 * @brief HTTP url-encoded parameter lexer
 *
 * HTML forms sends their data in two ways:
 *  - using GET method and part of the URL after '?' character
 *  - using POST method and any standard commonly accepted by browsers
 *    like XML or url-encoding (same as in GET) which is specified in
 *    Content-Type header
 *
 * This Lexer is intended to be used with raw url-encoded string i. e.
 * either what's left in the buffer after parsing URL up to the '?'
 * character or what's left in the buffer after parsing HTTP POST headers
 */
class Lexer {
public:
    Lexer(BufferedInput& source) : source(source) {}
    Token getToken();

private:
    BufferedInput& source;

    void finishText(Token& token);
    void finishChar(Token& token);
};

}
}
