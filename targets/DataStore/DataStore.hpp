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
using namespace table;
using namespace pqxx;
namespace store {

void test();

#define TIMEOUT 0.05

class DataStore{
private:
    string connectionString;
public:
    DataStore();
    DataStore(const string & connection);
    Table execQuery(string sql);
    void setConnectionString(const string & connection);
};
}
