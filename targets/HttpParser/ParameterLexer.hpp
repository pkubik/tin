/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include "BufferedSource.hpp"

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
 * @brief Http protocol lexer
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
