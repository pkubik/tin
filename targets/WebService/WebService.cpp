/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "WebService.hpp"

using namespace server;

WebService::WebService()
    : handler()
    , server(handler, 7777)
{
    //configuration.setConfiguration();
    //std::string connString = configuration.getConnctionString();
    //handler.getDataBase().setConnectionString(connString);
    //handler.getDataBase().setConnectionString(configuration.getConnctionString());
    //handler.setStartingTable(configuration.getStartingTable());
}

void WebService::work()
{

    server.start();

}

