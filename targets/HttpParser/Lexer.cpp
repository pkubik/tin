/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "Lexer.hpp"

namespace parser {
namespace http {

namespace {

bool isOther(char c) {
    return c != ':' && !::isspace(static_cast<int>(c));
}

}

Token Lexer::getToken() {
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

void Lexer::finishWord(Token& token) {
    char n = source.peekChar();
    while (isOther(n)) {
        token.value += n;
        source.getChar();
        n = source.peekChar();
    }
}

void Lexer::finishCrlf(Token& token) {
    char n = source.peekChar();
    if (n == '\n') {
        token.value += n;
        source.getChar();
    }
}

}
}
