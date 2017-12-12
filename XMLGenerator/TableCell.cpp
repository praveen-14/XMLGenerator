#include <TableCell.h>

TableCell::TableCell()
{
}
TableCell::~TableCell()
{

}

void TableCell::setValue(QString data)
{
    this->value = data;
}

void TableCell::setField(FieldInfo *field)
{
    this->field = field;
}

FieldInfo* TableCell::getField()
{
    return this->field;
}

QString TableCell::getValue()
{
    return value;
}
