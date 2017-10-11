#ifndef TABLECELL_H
#define TABLECELL_H

#include <TableRow.h>

class TableCell
{
    public:
        TableCell(int row,int column);
        ~TableCell();
        void setValue(QString data);
        void setValueSet(QList<QString> valueSet);
//        void setNullable(bool nullable){this->nullable = nullable;}
        QString* getValue(){
            return &value;
        }
        QList<QString> getValueSet(){
            return valueSet;
        }
//        bool getNullable(){return this->nullable;}
    private:
        QList<QString> valueSet;
        QString value;
        int columnIndex;
        int rowIndex;
//        bool nullable = false;

};
#endif // TABLECELL_H
