/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include <sstream>

#include "HttpParser/BufferedSource.hpp"
#include "HttpParser/HttpLexer.hpp"
#include "HttpParser/UriLexer.hpp"

using namespace parser;

class SourceWrapper : public SourceReader {
private:
    std::istringstream& source;

public:
    SourceWrapper(std::istringstream& source) : source(source) {}

    virtual int read(char* buffer, size_t length) {
        source.read(buffer, length);
        return source.gcount();
    }
};

TEST_CASE( "Http lexer scanning whole input", "[Parser::HTTP]" ) {
    using namespace parser::http;

    std::istringstream input("GET \n"
                             "/res HTTP/1.1\r\n"
                             "Accept: text/plain; q=0.5, text/html\r\n"
                             "\r\n");

    const auto tokens = {
        Token{Token::Type::WORD, "GET"},
        Token{Token::Type::BLANK, " "},
        Token{Token::Type::CRLF, "\n"},
        Token{Token::Type::WORD, "/res"},
        Token{Token::Type::BLANK, " "},
        Token{Token::Type::WORD, "HTTP/1.1"},
        Token{Token::Type::CRLF, "\r\n"},
        Token{Token::Type::WORD, "Accept"},
        Token{Token::Type::COLON, ":"},
        Token{Token::Type::BLANK, " "},
        Token{Token::Type::WORD, "text/plain;"},
        Token{Token::Type::BLANK, " "},
        Token{Token::Type::WORD, "q=0.5,"},
        Token{Token::Type::BLANK, " "},
        Token{Token::Type::WORD, "text/html"},
        Token{Token::Type::CRLF, "\r\n"},
        Token{Token::Type::CRLF, "\r\n"},
    };

    SourceWrapper source(input);
    BufferedInput bi(source, 3);
    Lexer lexer(bi);

    for (auto& expToken : tokens) {
        const auto& token = lexer.getToken();
        CHECK(token.type == expToken.type);
        CHECK(token.value == expToken.value);
    }
}

TEST_CASE( "URI lexer scanning whole input", "[Parser::URI]" ) {
    using namespace parser::uri;

    std::istringstream input("/table/{from}-{to}/");

    const auto tokens = {
        Token{Token::Type::SLASH, "/"},
        Token{Token::Type::TEXT, "table"},
        Token{Token::Type::SLASH, "/"},
        Token{Token::Type::VAR, "from"},
        Token{Token::Type::TEXT, "-"},
        Token{Token::Type::VAR, "to"},
        Token{Token::Type::SLASH, "/"},
        Token{Token::Type::END, ""},
    };

    SourceWrapper source(input);
    BufferedInput bi(source, 10);
    Lexer lexer(bi);

    for (auto& expToken : tokens) {
        const auto& token = lexer.getToken();
        CHECK(token.type == expToken.type);
        CHECK(token.value == expToken.value);
    }
}

TEST_CASE( "URI lexer scanning corrupted input", "[Parser::URI]" ) {
    using namespace parser::uri;

    std::istringstream input("/table/{from}-{to/notparsed");

    const auto tokens = {
        Token{Token::Type::SLASH, "/"},
        Token{Token::Type::TEXT, "table"},
        Token{Token::Type::SLASH, "/"},
        Token{Token::Type::VAR, "from"},
        Token{Token::Type::TEXT, "-"},
        Token{Token::Type::ERROR, "to/"},
    };

    SourceWrapper source(input);
    BufferedInput bi(source, 10);
    Lexer lexer(bi);

    for (auto& expToken : tokens) {
        const auto& token = lexer.getToken();
        CHECK(token.type == expToken.type);
        CHECK(token.value == expToken.value);
    }
}
