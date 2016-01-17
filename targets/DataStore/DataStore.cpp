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
#include <string>
#include <iostream>


namespace store {
using namespace pqxx;
using namespace table;

DataStore::DataStore(){}

DataStore::DataStore(const std::string &connection){
   connectionString= connection;
}

/*sets DataStore connection string*/
void DataStore::setConnectionString(const std::string &connection){
    connectionString= connection;
}

/*execute sql statement from web service*/
Table DataStore::execQuery(const std::string & sql) const{
    connection conn(connectionString);

    if(!conn.is_open()){
        throw std::runtime_error("can't connect to database");
    }

    nontransaction N(conn);
    pipeline pipe(N);

    Table resultTable;

    const pipeline::query_id idQuery = pipe.insert(sql);

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

            //if first iteration get column names
            if(row == queryResult.begin())
            {
                resultTable.addColumnName(field->name());

            }
       }

       resultTable.addRow(tableRow);
     }
    return resultTable;
}

/*returns names of all tabels in database*/
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
/*if selected column is foreign key in selected table returns name of related tabel and column name in this tabel*/
std::tuple<bool,string,string> DataStore::relatedTable(std::string &tableName, std::string &columnName){
    connection conn(connectionString);

    if(!conn.is_open()){
        throw std::runtime_error("can't connect to database");
    }

    nontransaction N(conn);


    string fatSelect = "SELECT ccu.table_name AS foreign_table_name, ccu.column_name AS foreign_column_name "
                       "FROM information_schema.table_constraints AS tc "
                       "JOIN information_schema.key_column_usage AS kcu "
                       "ON tc.constraint_name = kcu.constraint_name "
                       "JOIN information_schema.constraint_column_usage AS ccu "
                       "ON ccu.constraint_name = tc.constraint_name "
                       "WHERE constraint_type = 'FOREIGN KEY' AND tc.table_name='"+ tableName +"' and kcu.column_name='" + columnName +"';";


    result resultQuery( N.exec( fatSelect));
    conn.disconnect();
    if(resultQuery.size()==0){
        return std::make_tuple(false,"","");
    }

    return std::make_tuple(true,resultQuery.at(0).at(0).c_str(),resultQuery.at(0).at(1).c_str());
}

}
