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
    std::tuple<bool,string,string> relatedTable(string &tableName,string &columnName);
    vector<string> getPrimaryKeyColumnName(string &tableName);
    Table execSelect(string sql,vector<string> order,int pageSize,int pageNr);
};
}
