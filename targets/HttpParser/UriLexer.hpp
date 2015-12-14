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

#include "BufferedInput.hpp"

namespace parser {
namespace uri {

struct Token {
    enum Type : char {
        TEXT,
        SLASH,
        VAR,
        ERROR,
        END
    };

    Type type;
    std::string value;
};

/**
 * @brief URI patterns lexer
 *
 * Note that this class can be only used by URI patterns parser,
 * which generates compiled URI pattern. Raw URIs extracted by
 * HTTP parser are to be parsed using pattern comparator.
 */
class Lexer {
public:
    Lexer(BufferedInput& source) : source(source) {}
    Token getToken();

private:
    BufferedInput& source;

    void finishText(Token& token);
    void finishVar(Token& token);
};

}
}
