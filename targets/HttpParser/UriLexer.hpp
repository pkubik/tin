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

#include "BufferedSource.hpp"

namespace parser {
namespace uri {

struct Token {
    enum Type {
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
