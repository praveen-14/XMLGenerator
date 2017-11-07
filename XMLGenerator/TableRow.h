#pragma once

#include <QList>
#include <TableCell.h>

class TableCell;

class TableRow
{
    public:
        TableRow(int row);
        ~TableRow();
        void addCell(TableCell *cell);
        QList<TableCell*>* getRowCells();
        void setRowIndex(int row);
        int getRowIndex();
    private:
        QList<TableCell*> rowCells;
        int rowIndex;
};
