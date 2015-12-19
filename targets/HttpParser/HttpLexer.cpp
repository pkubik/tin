/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpLexer.hpp"
#include "unordered_map"

namespace parser {
namespace http {

namespace {

bool isOther(char c) {
    return c != ':' && !::isspace(static_cast<int>(c)) && c != BufferedInput::END;
}

Keyword::Id getKeywordId(const std::string& buffer) {
    static const std::unordered_map<std::string, Keyword::Id> keywords = {
        {"GET", Keyword::Id::GET},
        {"POST", Keyword::Id::POST},
        {"HEAD", Keyword::Id::HEAD},
    };

    auto it = keywords.find(buffer);
    if (it != keywords.end()) {
        return it->second;
    } else {
        return Keyword::Id::NONE;
    }
}

}

std::unique_ptr<Token> Lexer::getToken() {
    const char c = source.getChar();

    if (c == source.END) {
        return std::unique_ptr<Token>(new Token{Token::Type::END});
    }

    if (c == ':') {
        return std::unique_ptr<Token>(new Token{Token::Type::COLON});
    }

    if (c == '?') {
        return std::unique_ptr<Token>(new Token{Token::Type::QMARK});
    }

    if (::isspace(static_cast<int>(c))) {
        if (::isblank(static_cast<int>(c))) {
            return std::unique_ptr<Token>(new Token{Token::Type::BLANK});
        } else {
            if (c == '\r') {
                finishCrlf();
            }

            return std::unique_ptr<Token>(new Token{Token::Type::CRLF});
        }
    }

    // Word or Keyword

    std::string buffer(1, c);
    finishWord(buffer);

    auto keyword = getKeywordId(buffer);
    if (keyword != Keyword::Id::NONE) {
        return std::unique_ptr<Token>(new Keyword(keyword));
    }

    return std::unique_ptr<Token>(new Word(std::move(buffer)));
}

void Lexer::finishWord(std::string& buffer) {
    char n = source.peekChar();
    while (isOther(n)) {
        buffer += n;
        source.getChar();
        n = source.peekChar();
    }
}

void Lexer::finishCrlf() {
    char n = source.peekChar();
    if (n == '\n') {
        source.getChar();
    }
}

}
}
