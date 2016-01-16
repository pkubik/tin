/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "DataStore.hpp"
#include <ctime>
using namespace pqxx;
using namespace table;
#include <string>
#include <iostream>

namespace store {
using namespace pqxx;

DataStore::DataStore(){}

DataStore::DataStore(const std::string &connection){
   connectionString= connection;
}

void DataStore::setConnectionString(const std::string &connection){
    connectionString= connection;
}

Table DataStore::execQuery(std::string sql){
    connection conn(connectionString);

    if(!conn.is_open()){
        throw std::runtime_error("can't connect to database");
    }

    nontransaction N(conn);
    pipeline pipe(N);

    Table resultTable;

    const pipeline::query_id idQuery = pipe.insert(sql);
    std::clock_t start = std::clock();

//    while(!pipe.is_finished(idQuery) )
//    {

//        if(((std::clock()-start)/(static_cast<double> (CLOCKS_PER_SEC))) > TIMEOUT)
//        {
//            pipe.cancel();
//            conn.disconnect();
//            throw std::runtime_error("query timeout");
//        }

//    }

    result queryResult= pipe.retrieve(idQuery);



    for (result::const_iterator row = queryResult.begin();
         row != queryResult.end();
         ++row)
     {
        vector<string> tableRow;
       for (result::tuple::const_iterator field = row->begin();
            field != row->end();
            ++field)
       {
            tableRow.push_back(field->c_str());

            if(row == queryResult.begin())
            {
                resultTable.addColumnName(field->name());

            }
       }

       resultTable.addRow(tableRow);
     }
    return resultTable;
}

vector<string> DataStore::getAllTables(){
    vector<string> tablesNames;
    connection conn(connectionString);

    if(!conn.is_open()){
        throw std::runtime_error("can't connect to database");
    }

    nontransaction N(conn);

    result resultQuery( N.exec( "Select table_name from information_schema.tables where table_schema='public'" ));

    for (result::const_iterator i=resultQuery.begin(); i!=resultQuery.end();++i) {
           tablesNames.push_back(i[0].as<std::string>());
        }

    conn.disconnect();
    return tablesNames;
}
}
