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

    Token getToken() {
        if (!source) {
            return Token{Token::Type::END, ""};
        }

        const char c = source.getChar();

        if (c == ':') {
            return Token{Token::Type::COLON, std::string(1, c)};
        }

        if (::isspace(static_cast<int>(c))) {
            if (::isblank(static_cast<int>(c))) {
                return Token{Token::Type::BLANK, std::string(1, c)};
            } else {
                auto token = Token{Token::Type::CRLF, std::string(1, c)};

                if (c == '\r') {
                    finishCrlf(token);
                }

                return token;
            }
        }

        auto token = Token{Token::Type::WORD, std::string(1, c)};
        finishWord(token);
        return token;
    }

private:
    BufferedInput& source;

    void finishWord(Token& token) {
        char n = source.peekChar();
        while (n != ':' && !::isspace(static_cast<int>(n))) {
            token.value += n;
            source.getChar();
            n = source.peekChar();
        }
    }

    void finishCrlf(Token& token) {
        char n = source.peekChar();
        if (n == '\n') {
            token.value += n;
            source.getChar();
        }
    }
};

}
}
