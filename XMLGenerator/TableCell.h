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


        QString getValue();
        FieldInfo* getField();
        int getColumnIndex();
    private:
        QString value;
        FieldInfo *field;

};
#endif // TABLECELL_H
