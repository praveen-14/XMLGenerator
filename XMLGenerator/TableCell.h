#ifndef TABLECELL_H
#define TABLECELL_H

#include <TableRow.h>
#include <FieldInfo.h>

class TableCell
{
    public:
        TableCell();
        ~TableCell();
        void setValue(QString data);

        void setField(FieldInfo *field);

        void setColumnIndex(int column);

//        void setRowIndex(int row);

        QString getValue();
        FieldInfo* getField();
//        int getRowIndex();
        int getColumnIndex();
    private:
        QString value;
        FieldInfo *field;
//        int columnIndex;
//        int rowIndex;

};
#endif // TABLECELL_H
