/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

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

class FancyHandler : public Handler {
public:
    virtual Response handle(const Request& request, RequestError error);

private:
    // internal handlers not required by the server API
    Response handleRequestError() const ;
    Response handleGeneralError(const Request& request) const ;
    Response handle404Error(const Request& request) const ;
    Response handleSuccess(const Request& request) const ;

    /*
     * HTML templater call.
     * Note: original templater API should be different! (accept additional parameters)
     */
    std::string generateHtmlTemplate(const std::string& message) const ;

    /*
     * Handles simple echo response.
     */
    void fillSimpleResponse(Response& response, const Request& request, const std::string& message) const ;

    /*
     * Checks if the client accepts HTML response.
     */
    bool acceptsHtml(const Request& request) const ;
};
