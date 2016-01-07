/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "HttpServer/HttpServer.hpp"
#include "HttpParser/HttpParser.hpp"
#include "Common/Logger.hpp"

using namespace server;

/*
 * Sample HTTP request handler.
 *
 * Echoes text provided as request resource '/echo/<text>'.
 * This is an example. Final implementation might not be stateless.
 */
class EchoHandler : public Handler {
public:
    virtual Response handle(const Request& request) {
        LOGT("Echo handler handles request...");

        if (request.getMethod() != Request::GET || request.getVersion() != "HTTP/1.1") {
            return handleGeneralError(request);
        }

        std::string echo = "/echo/";
        if (request.getResource().compare(0, echo.length(), echo) != 0) {
            return handle404Error(request);
        }

        return handleSuccess(request);
    }

    virtual Response handleError() {
        return handleRequestError();
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

int main()
{
    EchoHandler handler;
    Server server(handler, 7777);
    server.start();

    LOGT("trace");
    LOGD("debug");
    LOGI("info");
    LOGW("warning");
    LOGE("error");
    LOGH("help");

    return 0;
}
