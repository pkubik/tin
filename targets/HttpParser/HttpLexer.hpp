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
#include <memory>

#include "BufferedInput.hpp"

namespace parser {
namespace http {

struct Token {
    enum Type {
        KEYWORD,
        WORD,
        COLON,
        QMARK,
        BLANK, // space or tab
        CRLF,  // line feed in any format
        END
    };

    Type type;

    static std::unique_ptr<Token> create(Type type) {
        return std::unique_ptr<Token>(new Token{type});
    }

    template <typename T>
    T& as() { return static_cast<T&>(*this); }
};

struct Keyword : Token {
    enum Id {
        NONE,
        GET,
        POST,
        HEAD,
    };

    Id id;

    Keyword(Id id) : Token{Token::Type::KEYWORD}, id(id) {}

    static std::unique_ptr<Keyword> create(Id id) {
        return std::unique_ptr<Keyword>(new Keyword{id});
    }
};

struct Word : Token {
    std::string value;

    Word(std::string&& value) : Token{Token::Type::WORD}, value(value) {}

    static std::unique_ptr<Word> create(std::string&& value) {
        return std::unique_ptr<Word>(new Word{std::move(value)});
    }
};


/**
 * @brief HTTP/1.1 protocol lexer
 *
 * This lexer is more permissive than actual HTTP standard.
 */
class Lexer {
public:
    Lexer(BufferedInput& source) : source(source) {}
    std::unique_ptr<Token> getToken();

private:
    BufferedInput& source;

    void finishWord(std::string& buffer);
    void finishCrlf();
};

}
}
