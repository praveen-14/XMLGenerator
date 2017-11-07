#ifndef TABLE_H
#define TABLE_H
#include <TableRow.h>
#include <QDebug>
#include <QHeaderView>
#include <CacheConfig.h>
#include <FieldInfo.h>

class TableRow;
class TableCell;
using namespace std;

class Table
{
    public:
        Table();
        ~Table();
        void addCell(int row, int column, QString value , FieldInfo *field);
        void addRow(TableRow *row);

        QString updateTableModel(int row, int column, QString value);

        void addAttribute(QString attribute);
        QList<QString>* getAllAttributes();

        void addField(QString field);
        QList<QString>* getAllFields();

        QString addAttributeToModel(FieldInfo *newField);
        QString addFieldToModel(QMap<QString,QString> *fieldData);

        QList<TableRow*>* getRows();
        QMap<TableCell*,QString>* getErrorsMap();

        QString trim(QString s);
        QString validate(QString value, FieldInfo *field);

        void removeAttribute(int index);
        void removeField(int index);

    private:
        QList<TableRow*> tableRows;
        QList<QString> allAttributes;
        QList<QString> allFields;
        QMap<TableCell*,QString> errorsMap;
};
#endif // TABLE_H
