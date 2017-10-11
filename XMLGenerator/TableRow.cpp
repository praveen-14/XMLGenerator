#include <TableRow.h>
#include <TableCell.h>

TableRow::TableRow(int row) :  rowIndex(row)
{

}

TableRow::~TableRow()
{

}

void TableRow::addCell(TableCell *cell)
{
    this->getRowCells()->push_back(*cell);
//    TableCell *cell = new TableCell(this->getRowIndex(),column);
//    foreach(QString data , dataList)
//    {
//        cell->addData(data);
//    }
//    this->getRowCells()->push_back(cell);
}
