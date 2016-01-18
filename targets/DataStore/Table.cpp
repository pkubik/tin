/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "Table.hpp"

namespace table {

Table::Table(){
    columnCount=0;
    rowCount=0;
}

/*returns how many rows is in table*/
int Table::tableSize(){
    return rowCount;
}

/*returns how many columns has single row*/
int Table::rowSize(){
    return columnCount;
}

/*returns names of colmns*/
vector<string> Table::getColumnsNames(){
    return columnsNames;
}

/*returns row selected by argument*/
vector<string> Table::getRow(int i){
    return queryResult[i];
}

/*adds row to table*/
void Table::addRow(vector<std::string> &row){
    ++rowCount;
    queryResult.push_back(row);
}

/*removes selected row*/
void Table::delRow(int i){
    --rowCount;
    queryResult.erase(queryResult.begin() + i);
}

/*adds column name*/
void Table::addColumnName(std::string columName){
    ++columnCount;
    columnsNames.push_back(columName);
    }

}
