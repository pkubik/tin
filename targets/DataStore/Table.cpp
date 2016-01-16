#include "Table.hpp"

namespace table {

int Table::tableSize(){
    return rowCount;
}

int Table::rowSize(){
    return columnCount;
}

vector<string> Table::getColumnsNames(){
    return columnsNames;
}

vector<string> Table::getColumnsTypes(){
    return columnsType;
}

vector<string> Table::getRow(int i){
    return queryResult[i];
}

void Table::addRow(vector<std::string> &row){
    ++rowCount;
    queryResult.push_back(row);
}

void Table::addColumnName(std::string columName){
    ++columnCount;
    columnsNames.push_back(columName);
    //columnsType.push_back(columnType);
}
}
