/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include "MyConfiguration.hpp"

MyConfiguration::MyConfiguration()
{
    configFile = "res/config.ini";
}

void MyConfiguration::setConfiguration()
{
    findOption(host, "host");
    findOption(port, "port");
    findOption(userdb, "userdb");
    findOption(password, "password");
    findOption(dbname, "dbname");
    findOption(startingTable, "startingTable");
}

void MyConfiguration::findOption(std::string &option, std::string optName)
{
    std::ifstream ifs(configFile, std::ifstream::in);

    std::string line;
    while(std::getline(ifs, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            if (trim(key) == optName)
            {
                std::string value;
                if( std::getline(is_line, value) )
                    option = trim(value);
            }

        }
    }
    ifs.close();
}

void MyConfiguration::findOption(int &option, std::string optName)
{
    std::ifstream ifs(configFile, std::ifstream::in);

    std::string line;
    while(std::getline(ifs, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            if (trim(key) == optName)
            {
                std::string value;
                if( std::getline(is_line, value) )
                    option = std::stoi(trim(value));
            }

        }
    }
    ifs.close();
}

std::string MyConfiguration::getConnctionString()
{
    //"user=tin host=czadzik24.pl port=5432 password=haslo dbname=tin"
    return "user=" + userdb + " host=" + host + " port=" + std::to_string(port) + " password=" + password + " dbname=" + dbname;
}
