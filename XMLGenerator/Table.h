#ifndef TABLE_H
#define TABLE_H
#include <TableRow.h>
#include <QDebug>
#include <QHeaderView>

class TableRow;
class TableCell;
using namespace std;

class Table
{
    public:
        Table();
        ~Table();
        void addCell(int row , int column , QString value, QList<QString> fieldValues);
        void addRow(TableRow *row);

        void addAttribute(QString attribute);
        QList<QString>* getAllAttributes(){return &allAttributes;}

        void addField(QString field);
        QList<QString>* getAllFields(){ return &allFields;}

        QMap<QString,QList<QString>>* addAttributeToModel(QMap<QString,QString> *attributeData);
        QMap<QString,QString>* addFieldToModel(QMap<QString,QString> *fieldData,QMap<int, QList<QString>> *fieldValues);

        QList<TableRow>* getRows(){ return &tableRows;}

    private:
        QList<TableRow> tableRows;
//        int noOfRows;
//        int noOfColumns;
        QList<QString> allAttributes;
        QList<QString> allFields;
};
#endif // TABLE_H
