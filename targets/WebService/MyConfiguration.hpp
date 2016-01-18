/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

class MyConfiguration{
public:
    MyConfiguration();

    void setConfiguration();
    std::string getConnctionString();
    std::string getStartingTable() const;
    std::string getRootResDir() const;
    std::string getPageSize() const;

private:
    std::string configFile;

    std::string host;
    int port;
    std::string userdb;
    std::string password;
    std::string dbname;
    
    std::string rootResDir;
    std::string startingTable;
    std::string pageSize;

    void findOption(std::string &option, std::string optionName);
    void findOption(int &option, std::string optionName);

    /*
    *  Poniższa funkcja pochodzi z zasobów sieci. Służy obcinaniu stringa z białych znaków na początku i końcu.
    */
    inline std::string trim(const std::string &s)
    {
        auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c); });
        auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c); }).base();
        return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
    }

};
