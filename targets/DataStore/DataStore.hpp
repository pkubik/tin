/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include <pqxx/pqxx>
#include "Table.hpp"
#include <string>
using namespace table;
using namespace pqxx;
namespace store {


class DataStore{
private:
    string connectionString;
public:
    DataStore();
    DataStore(const string & connection);
    Table execQuery(string sql);
    void setConnectionString(const string & connection);
    vector<string> getAllTables();
    std::tuple<bool,string,string> relatedTable(string &tableName,string &columnName);
};
}
