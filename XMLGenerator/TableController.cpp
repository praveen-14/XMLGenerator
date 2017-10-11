#include <TableController.h>

TableController::TableController(QString filePath)
{
//    this->filePath = filePath;
}

TableController::~TableController()
{

}

//void createTable(QList<QList<QString>,QList<QString>,QList<QList<QString>>> XMLData)
//{

//}


void TableController::populateTableModel(Table *table , XMLFileData data)
{
    QMap<int, QList<QString>> fieldValues = MainWindow::mainWindow->getFileController()->loadFieldValues(data.allAttributes);
    foreach(QString columnHeader , data.allAttributes)
    {
        table->addAttribute(columnHeader);
    }
    foreach(QString rowHeader , data.allFields)
    {
        table->addField(rowHeader);
    }
    for(int row=0; row<data.tableData.length(); row++){
        for(int column=0; column<data.tableData.at(row).length(); column++){
            QString value = data.tableData.at(row).at(column);
            QList<QString> valueSet;
            if(fieldValues.find(column) != fieldValues.end()){
                valueSet = (fieldValues.find(column)).value();
            }
//            if(fieldValues.contains(column)){
//                if(!(*(fieldValues.find(column))).contains(value)){
//                    (*(fieldValues.find(column))).prepend(value);
//                    valueSet =(*(fieldValues.find(column)));
//                }
//            }
            table->addCell(row,column,value,valueSet);
        }
    }
//    foreach (QList<QString> rowData, data.tableData) {
//        foreach (QString cellValue, rowData) {
//            int column = rowData.indexOf(cellValue);
//            int row = data.tableData.indexOf(rowData);
////            qDebug() << row;
////            qDebug() << column;
//            table->addCell(row,column,cellValue,fieldValues);
//        }
//    }
}

QTableWidget* TableController::createTableView(QString fileName)
{
    Table *table = this->getDataTable();
//    QGridLayout *gridLayout = frame->findChild<QGridLayout*>("gridLayout");
//    QTabWidget *tabWidget = frame->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = this->getTableView();
    tableWidget->setProperty("tableController",qVariantFromValue((void*)this));
    tableWidget->setProperty("fileName",fileName);
    tableWidget->verticalHeader()->setSectionsMovable(true);
    tableWidget->verticalHeader()->setDragEnabled(true);
    tableWidget->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setRowCount(table->getAllFields()->size());
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    tableWidget->setVerticalHeaderLabels(*(table->getAllFields()));
    for ( QList<TableRow>::iterator tableRow = table->getRows()->begin(); tableRow != table->getRows()->end(); ++tableRow)
    {
        for ( QList<TableCell>::iterator rowCell = (*tableRow).getRowCells()->begin(); rowCell != (*tableRow).getRowCells()->end(); ++rowCell)
        {
            if((*rowCell).getValueSet().length() > 0){
                QComboBox *comboBox = new QComboBox();
//                comboBox->addItem("N/A");
                int currentIndex = 0;
                QList<QString> valueSet = (*rowCell).getValueSet();
                for(int i=0;i<valueSet.length(); i++){
//                foreach (QString value, (*rowCell).getValueSet()) {
                    comboBox->addItem(valueSet.at(i));
                    if(valueSet.at(i).toStdString().compare((*((*rowCell).getValue())).toStdString()) == 0){
                        currentIndex = i;
                    }
                }
                comboBox->setCurrentIndex(currentIndex);
                tableWidget->setCellWidget(tableRow - table->getRows()->begin(),rowCell - (*tableRow).getRowCells()->begin(), comboBox);
            }else{
                tableWidget->setItem(tableRow - table->getRows()->begin(),rowCell - (*tableRow).getRowCells()->begin(),new QTableWidgetItem(*((*rowCell).getValue())));
            }
        }
    }
//    tabWidget->addTab(tableWidget,fileName);
//    this->setTabIndex(tabWidget->indexOf(tableWidget));
//    tabWidget->setCurrentIndex(tabWidget->indexOf(tableWidget));
//    frame->setLayout(gridLayout);
    return tableWidget;
}

void TableController::addAttributeToTableView(QString attributeName, QList<QString> valueSpace){
    QTableWidget *tableWidget = this->getTableView();
    Table *table = this->getDataTable();
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    for(int row=0; row<table->getAllFields()->count(); row++){
        if(valueSpace.count()>0){
            QComboBox *comboBox = new QComboBox();
            for(int i=0;i<valueSpace.length(); i++){
                comboBox->addItem(valueSpace.at(i));
            }
            tableWidget->setCellWidget(row,table->getAllAttributes()->count()-1, comboBox);
        }
    }
}

void TableController::addAttributeToModel(QMap<QString,QString> *attributeData){
    dataTable.addAttributeToModel(attributeData);
}
