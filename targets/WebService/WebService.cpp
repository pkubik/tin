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
    , configuration()
    , dataBase("")
{
    //MyConfiguration configuration;
    configuration.setConfiguration();
    std::string connString = configuration.getConnctionString();
    dataBase.setConnectionString(connString);

}

void WebService::work()
{

    Table tabela = dataBase.execQuery("INSERT INTO GROUPS VALUES (16, 'Add by Krystian');");
    std::cout << "\nil wierszy: " << tabela.tableSize();
    server.start();

}

