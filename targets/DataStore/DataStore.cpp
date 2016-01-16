/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "DataStore.hpp"

using namespace pqxx;
using namespace table;
namespace store {

DataStore::DataStore(const std::string &connection){
   connectionString= connection;
}

Table DataStore::execQuery(std::string sql){
    asyncconnection conn(connectionString);
    if(!conn.is_open()){
        throw std::runtime_error("can't connect to database");
    }

    nontransaction N(conn);

    pipeline pipe(N);

    return Table();
}
}
