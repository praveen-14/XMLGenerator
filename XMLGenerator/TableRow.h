#pragma once

#include <QList>
#include <TableCell.h>

class TableCell;

class TableRow
{
    public:
        TableRow();
        ~TableRow();
        void addCell(TableCell *cell);
        QList<TableCell*>* getRowCells();
    private:
        QList<TableCell*> rowCells;
};
