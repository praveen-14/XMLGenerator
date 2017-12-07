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

//        void addNewEnum(QString newEnum);
        void removeEnum(int index);
        QList<QString>* getNewEnums();

        QString addAttributeToModel(FieldInfo *newField, CacheConfig *config);
        QString addFieldToModel(QMap<QString,QString> *fieldData, CacheConfig *config);

        QList<TableRow*>* getRows();
        QMap<TableCell*,QString>* getErrorsMap();

        QString trim(QString s);
        QString validate(QString value, FieldInfo *field);

        void removeAttribute(int index);
        void removeField(int index);

        QList<QString>* getAvailableAttributes();
        void setAvailableAttributes(QList<QString> *availableAttributes);

    private:
        QList<TableRow*> tableRows;
        QList<QString> allAttributes;
        QList<QString> allFields;
        QMap<TableCell*,QString> errorsMap;
        QList<QString> newEnums;
        QList<QString> availableAttributes;
};
#endif // TABLE_H
