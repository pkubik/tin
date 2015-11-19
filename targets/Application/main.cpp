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

namespace server = HttpServer;
namespace parser = HttpParser;

int main()
{
    parser::hello();
    server::hello();
    return 0;
}
