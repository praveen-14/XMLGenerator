#include <TableRow.h>
#include <TableCell.h>

TableRow::TableRow()
{

}

TableRow::~TableRow()
{

}

QList<TableCell*>* TableRow::getRowCells()
{
    return &rowCells;
}

void TableRow::addCell(TableCell *cell)
{
    this->getRowCells()->push_back(cell);
}
