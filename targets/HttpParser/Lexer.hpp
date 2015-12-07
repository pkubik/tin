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
namespace http {

struct Token {
    enum Type {
        WORD,
        COLON,
        BLANK, // space or tab
        CRLF,  // line feed in any format
        END
    };

    Type type;
    std::string value;
};

class Lexer {
public:
    Lexer(BufferedInput& source) : source(source) {}
    Token getToken();

private:
    BufferedInput& source;

    void finishWord(Token& token);
    void finishCrlf(Token& token);
};

}
}
