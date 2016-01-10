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
}

void WebService::work()
{
    server.start();
}
