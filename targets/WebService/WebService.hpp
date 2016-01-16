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
#include "FancyHandler.hpp"
#include "MyConfiguration.hpp"
#include "DataStore/DataStore.hpp"

using namespace server;

class WebService {
public:
    WebService();
    void work();

private:
    Server server;
    FancyHandler handler;
    MyConfiguration configuration;
    store::DataStore dataBase;
};
