/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "UriLexer.hpp"

namespace parser {
namespace uri {

namespace {

bool isText(char c) {
    return c != '{' && c != '}' && c != '/' && c != BufferedInput::END;
}

}

Token Lexer::getToken() {
    const char c = source.getChar();

    if (c == source.END) {
        return Token{Token::Type::END, ""};
    }

    if (c == '/') {
        return Token{Token::Type::SLASH, std::string(1, c)};
    }

    if (c == '}') {
        return Token{Token::Type::ERROR, std::string(1, c)};
    }

    if (c == '{') {
        auto token = Token{Token::Type::VAR, ""};
        finishVar(token);
        return token;
    }

    auto token = Token{Token::Type::TEXT, std::string(1, c)};
    finishText(token);
    return token;
}

void Lexer::finishText(Token& token) {
    char n = source.peekChar();
    while (isText(n)) {
        token.value += n;
        source.getChar();
        n = source.peekChar();
    }
}

void Lexer::finishVar(Token& token) {
    finishText(token);

    char n = source.getChar();
    if (n != '}') {
        token.type = Token::Type::ERROR;
        token.value += n;
    }
}

}
}

