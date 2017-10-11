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
        QList<TableCell>* getRowCells(){return &rowCells;}

        int* getRowIndex(){return &rowIndex;}
    private:
        QList<TableCell> rowCells;
        int rowIndex;
};
