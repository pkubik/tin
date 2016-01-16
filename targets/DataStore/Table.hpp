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
    vector<string> columnsType;
    vector<string> columnsNames;
    vector<vector<string>> queryResult;

public:
    int tableSize();
    int rowSize();
    vector<string> getColumnsNames();
    vector<string> getColumnsTypes();
    vector<string> getRow(int i);
    void addRow(vector<std::string> &row);
    void addColumnName(string columName);
};

}
