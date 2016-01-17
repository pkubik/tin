/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */
#include <vector>
#include<string>
using std::string;
using std::vector;

namespace table {
class Table{
private:
    int rowCount;
    int columnCount;
    vector<string> columnsNames;
    vector<vector<string>> queryResult;

public:
    Table();
    int tableSize();
    int rowSize();
    vector<string> getColumnsNames();
    vector<string> getRow(int i);
    void addRow(vector<std::string> &row);
    void addColumnName(string columName);
};

}
