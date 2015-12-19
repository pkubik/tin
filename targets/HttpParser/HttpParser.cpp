/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpParser.hpp"
#include "HttpLexer.hpp"

#include <iostream>

namespace parser {
namespace http {

namespace {

bool isText(const Token& token) {
    return token.type != Token::Type::CRLF && token.type != Token::Type::END;
}

}

Parser::Result Parser::parse(BufferedInput& input) {
    Result result;

    Parser parser(input, result.first, result.second);

    parser.parse();

    return result;
}

void Parser::parse() {
    if (!parseMethod()) {
        return;
    }

    if (!parseWhitespace()) {
        return;
    }

    if (token->type == Token::Type::WORD) {
        request.uri = token->as<Word>().value;
        token = lexer.getToken();
    } else {
        status.setError("Error parsing URI");
        return;
    }

    if (token->type == Token::Type::QMARK) {
        if (!parseParameters()) {
            return;
        }

        token = lexer.getToken();
        skipBlanks();
    } else {
        if (!parseWhitespace()) {
            return;
        }
    }

    if (token->type == Token::Type::WORD) {
        request.version = token->as<Word>().value;
        token = lexer.getToken();
    } else {
        status.setError("Error parsing HTTP version");
        return;
    }

    skipBlanks();

    if (!parseCRLF()) {
        return;
    }

    while (parseHeader());

    parseCRLF();

    if (request.method == Request::POST) {
        auto it = request.headers.find("Content-Type");
        if (it != request.headers.end() && it->second == "application/x-www-form-urlencoded") {
            parseParameters();
        }
    }
}

bool Parser::parseMethod() {
    bool ret = true;

    if (token->type == Token::Type::KEYWORD) {
        auto& keyword = token->as<Keyword>();
        if (keyword.id == Keyword::Id::GET) {
            request.method = Request::GET;
        } else if (keyword.id == Keyword::Id::POST)  {
            request.method = Request::POST;
        } else if (keyword.id == Keyword::Id::HEAD) {
            request.method = Request::HEAD;
        } else {
            status.setError("Unknown method type");
            ret = false;
        }
    } else {
        status.setError("Unknown method type");
        ret = false;
    }

    token = lexer.getToken();
    return ret;
}

bool Parser::parseHeader() {
    if (token->type != Token::Type::WORD) {
        if (token->type == Token::Type::CRLF) {
            // header list is empty
            return false;
        }
        status.setError("Error parsing header name");
        return false;
    }
    auto it = request.headers.insert(std::make_pair(token->as<Word>().value, ""));
    // NOTE: according to RFC2616 parser should know all possible header-fields and
    // their value structures and decide whether it is correct to allow header repetition

    token = lexer.getToken();

    skipBlanks();

    if (token->type != Token::Type::COLON) {
        status.setError("Colon between header name and value expected");
        return false;
    }
    token = lexer.getToken();

    skipBlanks();

    std::string value;
    if (!parseHeaderValue(value)) {
        return false;
    }

    it.first->second = value;

    return true;
}

bool Parser::parseHeaderValue(std::string& result) {
    for (;;) {
        while (isText(*token)) {
            if (token->type == Token::Type::WORD) {
                result += token->as<Word>().value;
            } else if (token->type == Token::Type::BLANK) {
                result += ' ';
            }
            token = lexer.getToken();
        }

        if (token->type == Token::Type::CRLF) {
            token = lexer.getToken();
            if (token->type == Token::Type::BLANK) {
                skipBlanks();
                result += ' ';
                continue;
            } else {
                return true;
            }
        } else {
            status.setError("Error parsing header value.");
            return false;
        }
    }

    return true;
}

bool Parser::parseWhitespace() {
    bool ret = false;

    while (token->type == Token::Type::BLANK || token->type == Token::Type::CRLF) {
        ret = true;
        token = lexer.getToken();
    }

    if (!ret) {
        status.setError("Whitespace expected");
    }

    return ret;
}

void Parser::skipBlanks() {
    while (token->type == Token::Type::BLANK) {
        token = lexer.getToken();
    }
}

bool Parser::parseCRLF() {
    bool ret = true;

    if (token->type != Token::Type::CRLF) {
        status.setError("CRLF expected");
        ret = false;
    }

    token = lexer.getToken();
    return ret;
}

bool Parser::parseParameters() {
    while (parseParameter()) {
        paramToken = parameterLexer.getToken();
        if (paramToken.type != parameter::Token::Type::AND) {
            if (paramToken.type == parameter::Token::Type::ERROR) {
                status.setError("Error while parsing parameters");
                return false;
            } else {
                return true;
            }
        }
    }

    status.setError("Error while parsing parameters");
    return false;
}

bool Parser::parseParameter() {
    paramToken = parameterLexer.getToken();

    if (paramToken.type == parameter::Token::Type::TEXT) {
        auto it = request.parameters.emplace(paramToken.value, "");

        paramToken = parameterLexer.getToken();

        if (paramToken.type == parameter::Token::Type::EQUAL) {
            paramToken = parameterLexer.getToken();

            if (paramToken.type == parameter::Token::Type::TEXT) {
                it.first->second = paramToken.value;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

}
}
