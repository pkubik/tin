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
#include "MyConfiguration.hpp"
#include "DataStore/DataStore.hpp"
#include "NLTemplate/NLTemplate.h"


using namespace server;

/*
 * Sample HTTP request handler.
 *
 * Echoes text provided as request resource '/echo/<text>'.
 * This is an example. Final implementation might not be stateless.
 */

class FancyHandler : public Handler {
public:  

    FancyHandler();
    virtual Response handle(const Request& request, RequestError error);

    store::DataStore& getDataBase();

private:
    MyConfiguration configuration;
    store::DataStore dataBase;

    // internal handlers not required by the server API
    Response handleRequestError() const ;
    Response handleGeneralError(const Request& request) const ;
    Response handle404Error(const Request& request) const ;
    Response handleSuccessEcho(const Request& request) const ;
    Response handleSuccessTable(const Request& request, const std::string tableName) const ;
    Response handleSuccessDetails(const Request& request, const std::string tableName) const ;
    Response handleSuccessMain(const Request& request) const ;
    Response handleFetchResource(const Request& request, const string& path) const ;

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
