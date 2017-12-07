#include "Change.h"

Change::Change(QObject *parent, ChangeType type, QString fieldName, QString attributeName, QString value) : QObject(parent)
{
    this->type = type;
    this->fieldName = fieldName;
    this->attributeName = attributeName;
    this->value = value;
}

Change::~Change()
{

}

ChangeType Change::getType()
{
    return this->type;
}

QString Change::getFieldName()
{
    return this->fieldName;
}

QString Change::getAttributeName()
{
    return this->attributeName;
}

QString Change::getValue()
{
    return this->value;
}

QString Change::getOldValue()
{
    return oldValue;
}

int Change::getSolutionRow()
{
    return this->solutionRow;
}

int Change::getSolutionColumn()
{
    return this->solutionColumn;
}

QString Change::getEnumName()
{
    return enumName;
}

void Change::setSolutionRow(int row)
{
    this->solutionRow = row;
}
void Change::setSolutionColumn(int column)
{
    this->solutionColumn = column;
}

void Change::setValue(QString value)
{
    this->value = value;
}

void Change::setOldValue(QString oldValue)
{
    this->oldValue = oldValue;
}

void Change::setEnumName(QString enumName)
{
    this->enumName = enumName;
}

QMap<QString,QString>* Change::getUpdateData()
{
    return &(this->updatedData);
}

void Change::addUpdate(QString key, QString value)
{
    this->updatedData.insert(key,value);
}

