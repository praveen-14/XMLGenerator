#include <TableCell.h>

TableCell::TableCell(int row, int column) : columnIndex(column) , rowIndex(row)
{
//    valueSet = *(new QList<QString>());
}
TableCell::~TableCell()
{

}

void TableCell::setValue(QString data){
    this->value = data;
}

void TableCell::setValueSet(QList<QString> valueSet){
    this->valueSet = valueSet;
}
