/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <cinttypes>
#include <string>
#include <map>

namespace parser {
namespace http {

struct Response {
    uint16_t code;
    std::map<std::string, std::string> headers;
    std::string message;

    // ensure it is not copied
    Response() = default;
    Response(Response&&) = default;
};

class Generator {
public:
    static std::string generate(const Response& response) {
        std::string text = std::to_string(response.code) + " HTTP/1.1\r\n";

        for (auto& header : response.headers) {
            text += header.first + " : " + header.second + "\r\n";
        }

        if (!response.message.empty()) {
            text += "Content-Length : " + std::to_string(response.message.length()) + "\r\n";
        }

        text += "\r\n";
        text += response.message;

        return text;
    }
};

}
}
