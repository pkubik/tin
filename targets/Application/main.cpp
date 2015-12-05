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

int main()
{
    server::hello();

    LOGT("trace");
    LOGD("debug");
    LOGI("info");
    LOGW("warning");
    LOGE("error");
    LOGH("help");

    return 0;
}
