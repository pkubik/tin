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

template <typename SourceType>
class Lexer {
public:
    struct Token {
        enum Type {
            WORD,
            COLON,
            BLANK,
            END
        };

        Type type;
        std::string value;
    };

    Lexer(BufferedInput<SourceType>& source) : source(source) {}

    Token getToken() {
        if (!source) {
            return Token{Token::Type::END, ""};
        }

        const char c = source.getChar();

        if (c == ':') {
            return Token{Token::Type::COLON, c};
        }

        if (::isblank(static_cast<int>(c))) {
            return Token{Token::Type::BLANK, c};
        }

        auto token = Token{Token::Type::WORD, c};

        char n = source.peekChar();
        while (n != ':' && !::isblank(static_cast<int>(n))) {
            token.value += n;
            source.getChar();
        }

        return token;
    }

private:
    BufferedInput<SourceType>& source;
};
