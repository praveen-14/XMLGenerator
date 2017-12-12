#include <Table.h>

Table::Table()
{

}
Table::~Table()
{
    qDebug() << "table deleted";
}

QMap<TableCell*,QString>* Table::getErrorsMap(){
    return &(this->errorsMap);
}

QList<QString>* Table::getAllFields(){
    return &allFields;
}

QList<QString>* Table::getAllAttributes(){
    return &allAttributes;
}

QList<TableRow*>* Table::getRows(){
    return &tableRows;
}

void Table::removeAttribute(int index)
{
    this->allAttributes.removeAt(index);
}

void Table::removeField(int index)
{
    this->allFields.removeAt(index);
}

QList<QString>* Table::getAvailableAttributes()
{
    return &availableAttributes;
}

void Table::setAvailableAttributes(QList<QString> *availableAttributes)
{
    this->availableAttributes = *availableAttributes;
}

QList<QString>* Table::getNewEnums()
{
    return &(newEnums);
}

void Table::removeEnum(int index)
{
    newEnums.removeAt(index);
}

void Table::addCell(int row, int column, QString value , FieldInfo *field)
{
    bool rowExists = false;
    QString unresolvedValue = value;
    QString resolvedValue = value;
    if(field->fieldType() == FieldType::DropDown)
    {
        bool valueFound = false;
        QMap<QString,QString>::iterator valueIterator = field->dropDownValMap()->begin();
        while(valueIterator != field->dropDownValMap()->end())
        {
            if(valueIterator.value().compare(unresolvedValue) == 0)
            {
                resolvedValue = valueIterator.key();
                valueFound = true;
                break;
            }
            valueIterator++;
        }
        if(!valueFound && value.compare("(Not Applicable)") != 0 && !newEnums.contains(value))
        {
            newEnums.push_back(value);
        }
    }
    if(row <= tableRows.size()-1)
    {
        TableCell* cell = new TableCell();
        cell->setValue(resolvedValue);
        cell->setField(field);
        tableRows.at(row)->addCell(cell);
        if(!field->nullable() && resolvedValue.compare(QString("")) == 0)
        {
            errorsMap.insert(cell,field->displayName() + " cannot be empty \n");
        }
        else if(field->isMandetory() && resolvedValue.compare(QString("(Not Applicable)")) == 0)
        {
            errorsMap.insert(cell,field->displayName() + " is mandatory \n");
        }
        rowExists = true;
    }
    if(!rowExists)
    {
        TableRow *newRow = new TableRow();
        TableCell *cell = new TableCell();
        cell->setValue(resolvedValue);
        cell->setField(field);
        newRow->addCell(cell);
        this->addRow(newRow);
        if(!field->nullable() && resolvedValue.compare("") == 0)
        {
            errorsMap.insert(cell,field->displayName() + " cannot be empty \n");
        }
        else if(field->isMandetory() && resolvedValue.compare("(Not Applicable)") == 0)
        {
            errorsMap.insert(cell,field->displayName() + " is mandatory \n");
        }
    }
}

void Table::addRow(TableRow *row)
{
    this->getRows()->push_back(row);
}

QString Table::updateTableModel(int row, int column, QString value)
{
    TableCell *cell = tableRows.at(row)->getRowCells()->at(column);
    FieldInfo *field = cell->getField();
    value = value.trimmed();
    QString errorString = validate(value, field);
    if(errorString.length() > 0)
    {
        return errorString;
    }
    else
    {
        tableRows.at(row)->getRowCells()->at(column)->setValue(value);
        qDebug() << "'" + field->displayName() + "' of '" + allFields.at(row) + "' updated to " + value;
        return QString("submitted");
    }
}

void Table::addField(QString field)
{
    if(this->getAllFields())
    {
        this->getAllFields()->push_back(field);
    }
    else
        qDebug() << " Error when adding fields ";

}

void Table::addAttribute(QString attributeName)
{
    this->getAllAttributes()->push_back(attributeName);
}

QString Table::addAttributeToModel(FieldInfo *newField, CacheConfig *config){
    QString errorString;
    QString attributeName = newField->name().trimmed();
    QString displayName = newField->displayName().trimmed();
    if(attributeName.size() != trim(attributeName).size())
    {
        errorString.append("Attribute name cannot contain spaces. \n");
    }
    if(attributeName.compare("") == 0)
    {
        errorString.append("Attribute name cannot be empty. \n");
    }
    else
    {
        newField->setName(attributeName);
    }
    if(displayName.compare("") == 0)
    {
        errorString.append("Display name cannot be empty. \n");
    }
    else
    {
        newField->setDisplayName(displayName);
    }
    if(newField->fieldType() == FieldType::Text && !newField->nullable())
    {
        QString defaultVal = newField->defaultVal().trimmed();
        if(defaultVal.compare("") == 0)
        {
            errorString.append("Default Value cannot be empty. \n");
        }
        else
        {
            newField->setDefaultVal(defaultVal);
        }
    }
    if(errorString.length() == 0)
    {
        this->addAttribute(newField->displayName());
        for(int row=0;row<this->getAllFields()->count();row++)
        {
            this->addCell(row,this->getAllAttributes()->count()-1,newField->defaultVal(),newField);
        }

        config->columnFieldList()->push_back(newField);
        return QString("submitted");
    }
    else
    {
        return errorString;
    }
}

QString Table::addFieldToModel(QMap<QString,QString> *fieldData, CacheConfig *config){
    QString fieldName;
    QString errorStringGlobal;
    QList<FieldInfo*> validatedFields;
    QList<QString> validatedValues;
    for(int column=0;column<this->getAllAttributes()->count();column++)
    {
        FieldInfo *attributeInfo;
        foreach (FieldInfo *field, *(config->columnFieldList()))
        {
            if(field->displayName().compare(this->getAllAttributes()->at(column)) == 0)
            {
                QString attributeName = field->name();
                attributeInfo = config->columnFieldList()->at(config->posForColumnName(attributeName));
            }
        }
        QString value = fieldData->find(attributeInfo->name()).value();
        QString trimmedValue = value.trimmed();
        if(attributeInfo)
        {
            QString errorString = validate(trimmedValue,attributeInfo);
            errorStringGlobal.append(errorString);
            if(errorString.length() == 0)
            {
                validatedFields.push_back(attributeInfo);
                validatedValues.push_back(trimmedValue);
                if(attributeInfo->name().compare(QString("displayName")) == 0)
                {
                    fieldName = trimmedValue;
                }
            }
        }
        else
        {
            qDebug() << "Failed to add field to model";
        }

    }
    if(errorStringGlobal.length() > 0)
    {
        return errorStringGlobal;
    }
    else
    {
        this->addField(fieldName);
        for(int column=0;column<this->getAllAttributes()->count();column++)
        {
            this->addCell(this->getAllFields()->count()-1,column,validatedValues.at(column),validatedFields.at(column));
        }
        return QString("submitted");
    }
}

QString Table::validate(QString value, FieldInfo *field){
    QString errorString;
    if(!field->nullable() && value.size() == 0)
    {
        errorString = errorString + "'" + field->displayName() + "'" + " cannot be empty. \n";
    }
    if(field->fieldType() == FieldType::Text)
    {
        bool correctString = true;
        for(int i = 0; i<value.length(); i++)
        {
            QChar c = value[i].toLatin1();
            if(!((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || (c == '_') || (c == ' ') || (c == '/') || (c == '-') || (c == ':')))
            {
                correctString = false;
                break;
            }
        }
        if(!correctString)
        {
            errorString = errorString + " Invalid value entered for '" + field->displayName() + "'\n";
        }
    }
    return errorString;
}

QString Table::trim(QString s)
{
    QString trimmedString;
    for(int i=0; i<s.length(); i++)
    {
        if(s[i].toLatin1() != ' ')
        {
            trimmedString.append(s[i]);
        }
    }
    return trimmedString;
}
