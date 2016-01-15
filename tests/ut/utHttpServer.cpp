/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include "HttpServer/HttpServer.hpp"
#include "Network/Abortable.hpp"
#include "Common/Logger.hpp"

#include <chrono>

using namespace server;

namespace {

/*
 * Sample HTTP request handler.
 *
 * Echoes text provided as request resource '/echo/<text>'.
 * This is an example. Final implementation might not be stateless.
 */
class EchoHandler : public Handler {
public:
    virtual Response handle(const Request& request, RequestError error) {
        LOGT("Echo handler handles request...");

        if (error != RequestError::NONE) {
            return handleRequestError();
        }

        if (request.getMethod() != Request::GET || request.getVersion() != "HTTP/1.1") {
            return handleGeneralError(request);
        }

        std::string echo = "/echo/";
        if (request.getResource().compare(0, echo.length(), echo) != 0) {
            return handle404Error(request);
        }

        return handleSuccess(request);
    }

private:
    // internal handlers not required by the server API

    Response handleRequestError() const {
        Response response;

        response.code = 400;
        response.headers["Content-Type"] = "text/plain";
        response.message = "Invalid request.";;

        return response;
    }

    Response handleGeneralError(const Request& request) const {
        Response response;

        response.code = 400;

        constexpr auto message = "Invalid request.";
        fillSimpleResponse(response, request, message);

        return response;
    }

    Response handle404Error(const Request& request) const {
        Response response;

        response.code = 404;

        constexpr auto message = "Requested resource not found.";
        fillSimpleResponse(response, request, message);

        return response;
    }

    Response handleSuccess(const Request& request) const {
        Response response;

        response.code = 200;

        constexpr char echo[] = "/echo/";
        fillSimpleResponse(response, request, request.getResource().substr(sizeof(echo) - 1));

        return response;
    }

    /*
     * HTML templater call.
     * Note: original templater API should be different! (accept additional parameters)
     */
    std::string generateHtmlTemplate(const std::string& message) const {
        auto ret = "<html>"
                   "<head>"
                   "<title>" + message + "</title>"
                   "<body>"
                   "<h1>Echo: " + message + "</h1>"
                   "</body>"
                   "</html>";

        return ret;
    }

    /*
     * Handles simple echo response.
     */
    void fillSimpleResponse(Response& response,
                            const Request& request,
                            const std::string& message) const {
        if (acceptsHtml(request)) {
            response.headers["Content-Type"] = "text/html";
            response.message = generateHtmlTemplate(message);
        } else {
            response.headers["Content-Type"] = "text/plain";
            response.message = message;
        }
    }

    /*
     * Checks if the client accepts HTML response.
     */
    bool acceptsHtml(const Request& request) const {
        auto it = request.getHeaders().find("Content-Type");
        if (it == request.getHeaders().end() ||
            it->second.find("text/html") != std::string::npos) {
            return true;
        }

        return false;
    }
};

}

TEST_CASE( "Server handle simple request", "[Server]" ) {
    using namespace network;

    const char requestText[] = "GET /echo/ABC HTTP/1.1\r\n"
                               "Host: 127.0.0.1:7777\r\n"
                               "User-Agent: Mozilla/5.0 Gecko/20100101 Firefox/43.0\r\n"
                               "Accept: text/html\r\n"
                               "Accept-Language: pl,en-US;q=0.7,en;q=0.3\r\n"
                               "\r\n";

    const char responseText[] = "200 HTTP/1.1\r\n"
                                "Content-Type : text/html\r\n"
                                "Content-Length : 68\r\n"
                                "\r\n"
                                "<html><head><title>ABC</title><body><h1>Echo: ABC</h1></body></html>";

    EchoHandler handler;
    Server server(handler, 0);
    const auto port = server.getPort();

    auto clientThread = std::thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        Socket client = Socket::connectINET("localhost", std::to_string(port));
        abortableWriteAll(client, requestText, sizeof(requestText), 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        char buffer[sizeof(responseText)];
        CHECK(!abortableReadAll(client, buffer, sizeof(buffer) - 1, 0));
        buffer[sizeof(buffer) - 1] = '\0';

        CHECK(std::string(buffer) == std::string(responseText));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        server.stop();
    });

    server.start();
    clientThread.join();
}

TEST_CASE( "Server handle POST request", "[Server]" ) {
    using namespace network;

    const char requestText[] = "POST /echo/ABC HTTP/1.1\r\n"
                               "Host: 127.0.0.1:7777\r\n"
                               "Content-Type: application/x-www-form-urlencoded\r\n"
                               "Content-Length: 68\r\n"
                               "Accept: text/html\r\n"
                               "\r\n"
                               "name=Gordon+Czapiger&specials="
                               "%2B%21%40%23%24%25%5E%26*%28%29%7B%7D1";

    // POST request isn't implemented in ECHO example
    const char responseText[] = "400 HTTP/1.1\r\n"
                                "Content-Type : text/plain\r\n"
                                "Content-Length : 16\r\n"
                                "\r\n"
                                "Invalid request.";

    EchoHandler handler;
    Server server(handler, 0);
    const auto port = server.getPort();

    auto clientThread = std::thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        Socket client = Socket::connectINET("localhost", std::to_string(port));
        abortableWriteAll(client, requestText, sizeof(requestText), 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        char buffer[sizeof(responseText)];
        CHECK(!abortableReadAll(client, buffer, sizeof(buffer) - 1, 0));
        buffer[sizeof(buffer) - 1] = '\0';

        CHECK(std::string(buffer) == std::string(responseText));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        server.stop();
    });

    server.start();
    clientThread.join();
}
