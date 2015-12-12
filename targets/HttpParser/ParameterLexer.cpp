/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "ParameterLexer.hpp"
#include "Exception.hpp"

namespace parser {
namespace parameter {

namespace {

bool isText(char c) {
    return c != '=' && c != '&' && c != BufferedInput::END;//TODO: fix all
}

}

Token Lexer::getToken() {
    const char c = source.getChar();

    if (c == source.END) {
        return Token{Token::Type::END, ""};
    }

    if (c == '=') {
        return Token{Token::Type::EQUAL, std::string(1, c)};
    }

    if (c == '&') {
        return Token{Token::Type::AND, std::string(1, c)};
    }

    auto token = Token{Token::Type::TEXT, std::string(1, c)};
    finishText(token);
    return token;
}

void Lexer::finishText(Token& token) {
    if (token.value.back() == '%') {
        finishChar(token);
    }

    char n = source.peekChar();

    while (isText(n)) {
        if (n == '+') {
            token.value += " ";
            source.getChar();
        } else if (n == '%') {
            token.value += n;
            source.getChar();
            finishChar(token);
        } else {
            token.value += n;
            source.getChar();
        }

        n = source.peekChar();
    }
}

void Lexer::finishChar(Token& token) {
    std::string ch = "0xAB";
    ch[2] = source.getChar();
    ch[3] = source.getChar();
    unsigned code = 0;

    try {
        code = std::stoul(ch, nullptr, 16);

        if (code == 0) {
            code = '#';
            throw std::invalid_argument("std::stoul returned 0.");
        }
    } catch (std::invalid_argument& e) {
        // unrecognized character code, not critical
        token.type = Token::Type::ERROR;
    }

    token.value.back() = static_cast<char>(code);
}

}
}
