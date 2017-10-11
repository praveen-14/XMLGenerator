#include <Table.h>

Table::Table()
{

}
Table::~Table()
{

}
void Table::addCell(int row , int column , QString value, QList<QString> fieldValues){
    bool rowExists = false;
//    QList<TableRow*> *rowArrayPointer = this->getRows();
//    while(rowArrayPointer < rowArrayPointer + this->getRows()->size())
    for ( QList<TableRow>::iterator tableRow = this->getRows()->begin(); tableRow != this->getRows()->end(); ++tableRow)
    {
        if(*(tableRow->getRowIndex())==row)
        {
            TableCell* cell = new TableCell(row,column);
            cell->setValue(value);
            cell->setValueSet(fieldValues);
            tableRow->addCell(cell);
            rowExists = true;
            break;
        }
//        rowArrayPointer++;
    }
    if(!rowExists)
    {
        TableRow *newRow = new TableRow(row);
        TableCell *cell = new TableCell(row,column);
        cell->setValue(value);
        cell->setValueSet(fieldValues);
        newRow->addCell(cell);
        this->addRow(newRow);
    }
}

void Table::addRow(TableRow *row)
{
    this->getRows()->push_back(*row);
}

void Table::addField(QString field)
{
    this->getAllFields()->push_back(field);
}

void Table::addAttribute(QString attributeName)
{
    this->getAllAttributes()->push_back(attributeName);
}

void Table::addAttributeToModel(QMap<QString,QString> *attributeData){
    QList<QString> rawValueSet;
    QList<QString> valueSet;
    QString attributeName = (attributeData->find("attributeName")).value();
    this->addAttribute(attributeName);
    if(attributeData->find("valueSpace").value().trimmed().length()>0){
        rawValueSet = attributeData->find("valueSpace").value().trimmed().split(",",QString::SkipEmptyParts);
    }
    foreach (QString string, rawValueSet) {
        if(string.trimmed()!=""){
            valueSet.append(string.trimmed());
        }
    }
//    this->getFileController()->updateFieldValues(attributeName,valueSet);
//    if(attributeData->find("nullable").value()){
//        valueSet.prepend("");
//    }
    for(int row=0;row<this->getAllFields()->count();row++){
        if(valueSet.count()>0){
            this->addCell(row,this->getAllAttributes()->count()-1,valueSet[0],valueSet);
        }else{
            this->addCell(row,this->getAllAttributes()->count()-1,"",valueSet);
        }
    }

}
