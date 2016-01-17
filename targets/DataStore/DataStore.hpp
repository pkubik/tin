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

namespace store {
using namespace table;
using namespace pqxx;


class DataStore{
private:
    string connectionString;
public:
    DataStore();
    DataStore(const string & connection);
    Table execQuery(const string & sql) const;
    void setConnectionString(const string & connection);
    vector<string> getAllTables();
    std::tuple<bool,string,string> relatedTable(const string &tableName,const string &columnName) const;
    vector<std::string> getPrimaryKeyColumnName(const std::string &tableName) const;
    Table execSelect(const std::string &sql, vector<std::string> order = vector<string>(), int pageSize = -1, int pageNr = -1) const;

};
}
