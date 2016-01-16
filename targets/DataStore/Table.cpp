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
}
