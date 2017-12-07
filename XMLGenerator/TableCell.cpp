#include <TableCell.h>

TableCell::TableCell()
{
//    valueSet = *(new QList<QString>());
}
TableCell::~TableCell()
{

}

void TableCell::setValue(QString data){
    this->value = data;
}

void TableCell::setField(FieldInfo *field){
    this->field = field;
}

//void TableCell::setColumnIndex(int column){
//    this->columnIndex = column;
//}

//void TableCell::setRowIndex(int row){
//    this->rowIndex = row;
//}

FieldInfo* TableCell::getField(){
    return this->field;
}

QString TableCell::getValue(){
    return value;
}

//int TableCell::getRowIndex(){
//    return this->rowIndex;
//}

//int TableCell::getColumnIndex(){
//    return this->columnIndex;
//}
