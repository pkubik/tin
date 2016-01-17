/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "catch/catch.hpp"

#include "DataStore/DataStore.hpp"

using namespace store;
using namespace table;

TEST_CASE( "Test tables' info accessing", "[DataStore]" ) {
    DataStore ds("user=tin host=czadzik24.pl port=5432 password=haslo dbname=tin");

    //table names acces
    std::vector<std::string> testTableNames = ds.getAllTables();
    CHECK( testTableNames.size() ==2);

    CHECK(testTableNames[0] == "groups");
    CHECK(testTableNames[1]== "students");

   //primary key access test
    CHECK(ds.getPrimaryKeyColumnName(testTableNames[0]).size() == 1);
    CHECK(ds.getPrimaryKeyColumnName(testTableNames[0])[0] == "group_id");

    CHECK(ds.getPrimaryKeyColumnName(testTableNames[1]).size() == 1);
    CHECK(ds.getPrimaryKeyColumnName(testTableNames[1])[0] == "student_id");

    //access to related table by FK
    //CHECK(ds.relatedTable("students","gr"))


}
