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

#include "HttpParser/BufferedInput.hpp"
#include "HttpParser/HttpParser.hpp"

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

TEST_CASE( "HTTP lexer scanning whole input", "[Parser::HTTP]" ) {
    using namespace parser::http;

    std::istringstream input("GET \n"
                             "/res HTTP/1.1\r\n"
                             "Accept: text/plain; q=0.5,\r\n"
                             "        text/html\r\n"
                             "\r\n");

    const std::unique_ptr<Token> tokens[] = {
        Keyword::create(Keyword::Id::GET),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::CRLF),
        Word::create("/res"),
        Token::create(Token::Type::BLANK),
        Word::create("HTTP/1.1"),
        Token::create(Token::Type::CRLF),
        Word::create("Accept"),
        Token::create(Token::Type::COLON),
        Token::create(Token::Type::BLANK),
        Word::create("text/plain;"),
        Token::create(Token::Type::BLANK),
        Word::create("q=0.5,"),
        Token::create(Token::Type::CRLF),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Token::create(Token::Type::BLANK),
        Word::create("text/html"),
        Token::create(Token::Type::CRLF),
        Token::create(Token::Type::CRLF)
    };

    SourceWrapper source(input);
    BufferedInput bi(source, 3);
    Lexer lexer(bi);

    for (auto& expToken : tokens) {
        const auto& token = lexer.getToken();
        CHECK(token->type == expToken->type);
        if (token->type == Token::Type::WORD) {
            CHECK(token->as<Word>().value == expToken->as<Word>().value);
        }
    }
}

TEST_CASE( "Parameter lexer scanning whole input", "[Parser::Parameter]" ) {
    using namespace parser::parameter;

    std::istringstream input("name=Gordon+Czapiger&specials="
                             "%2B%21%40%23%24%25%5E%26*%28%29%7B%7D1");

    const auto tokens = {
        Token{Token::Type::TEXT, "name"},
        Token{Token::Type::EQUAL, "="},
        Token{Token::Type::TEXT, "Gordon Czapiger"},
        Token{Token::Type::AND, "&"},
        Token{Token::Type::TEXT, "specials"},
        Token{Token::Type::EQUAL, "="},
        Token{Token::Type::TEXT, "+!@#$%^&*(){}1"},
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

TEST_CASE( "Parameter lexer scanning corrupted input", "[Parser::Parameter]" ) {
    using namespace parser::parameter;

    std::istringstream input("name=%ZZ");

    const auto tokens = {
        Token{Token::Type::TEXT, "name"},
        Token{Token::Type::EQUAL, "="},
        Token{Token::Type::ERROR, "#"},
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

TEST_CASE( "HTTP parser parsing whole GET request.", "[Parser::HTTP]" ) {
    using namespace parser::http;

    std::istringstream input("GET\n"
                             " /resource HTTP/1.1  \r\n"
                             "Connection: keep-alive\r\n"
                             "Multiline: a, b,\r\n"
                             "           c, d\r\n"
                             "\r\n");

    SourceWrapper source(input);
    BufferedInput bi(source, 10);
    auto result = Parser::parse(bi);

    REQUIRE(result.second == true);

    auto& req = result.first;
    CHECK(req.getMethod() == Request::GET);
    CHECK(req.getUri() == "/resource");
    CHECK(req.getVersion() == "HTTP/1.1");

    REQUIRE(req.getHeaders().size() == 2);
    CHECK(req.getHeaders().at("Connection") == "keep-alive");
    CHECK(req.getHeaders().at("Multiline") == "a, b, c, d");
}

TEST_CASE( "HTTP parser parsing whole GET request with parameters.", "[Parser::HTTP]" ) {
    using namespace parser::http;

    std::istringstream input("GET "
                             "/resource?name=Gordon+Czapiger&specials="
                             "%2B%21%40%23%24%25%5E%26*%28%29%7B%7D1 "
                             "HTTP/1.1  \r\n"
                             "Connection: keep-alive\r\n"
                             "\r\n");

    SourceWrapper source(input);
    BufferedInput bi(source, 10);
    auto result = Parser::parse(bi);

    REQUIRE(result.second == true);

    auto& req = result.first;
    CHECK(req.getMethod() == Request::GET);
    CHECK(req.getUri() == "/resource");
    CHECK(req.getVersion() == "HTTP/1.1");

    REQUIRE(req.getHeaders().size() == 1);
    CHECK(req.getHeaders().at("Connection") == "keep-alive");

    REQUIRE(req.getParameters().size() == 2);
    CHECK(req.getParameters().at("name") == "Gordon Czapiger");
    CHECK(req.getParameters().at("specials") == "+!@#$%^&*(){}1");
}
