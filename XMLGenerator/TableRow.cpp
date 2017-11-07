#include <TableRow.h>
#include <TableCell.h>

TableRow::TableRow(int row) :  rowIndex(row)
{

}

TableRow::~TableRow()
{

}

void TableRow::setRowIndex(int row){
    this->rowIndex = row;
}

QList<TableCell*>* TableRow::getRowCells(){
    return &rowCells;
}

int TableRow::getRowIndex(){
    return rowIndex;
}

void TableRow::addCell(TableCell *cell)
{
    this->getRowCells()->push_back(cell);
}
