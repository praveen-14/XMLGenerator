#include "Enum.h"

Enum::Enum(QString name)
{
    this->name = name;
}

QString Enum::getName()
{
    return name;
}

QMap<QString,QString>* Enum::getValues()
{
    return &(values);
}

void Enum::addValue(QString value, QString name)
{
    this->values.insert(value,name);
}
