/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include <QtSql/QtSql>
#include <pqxx/pqxx>
#include "Table.hpp"
using namespace table;
using namespace pqxx;
namespace store {

void test();
class DataStore{
private:
    string connectionString;
public:
    DataStore(const string & connection);
    Table execQuery(string sql);
};
}
