#include <TableController.h>

TableController::TableController(QWidget *tableWidgetParent)
{
    this->tableView = new QTableWidget(tableWidgetParent);
}

TableController::~TableController()
{
    qDebug() << "table controller deleted";
}

Table* TableController::getDataTable(){
    return &dataTable;
}

QTableWidget* TableController::getTableView(){
    return tableView;
}

QList<QList<QString>>* TableController::getMetaCacheData(){
    return &(this->metaCacheData);
}

QList<QList<QString>>* TableController::getMetaMessageData(){
    return &(this->metaMessageData);
}

QList<QList<QString>>* TableController::getDefaultSortList(){
    return &(this->defaultSortList);
}

void TableController::setMetaCacheData(QList<QList<QString>>* metaCacheData){
    this->metaCacheData = *metaCacheData;
}

void TableController::setMetaMessageData(QList<QList<QString>>* metaMessageData){
    this->metaMessageData = *metaMessageData;
}

void TableController::setDefaultSortList(QList<QList<QString>>* defaultSortList){
    this->defaultSortList = *defaultSortList;
}

QString TableController::updateMetaData(QList<QList<QList<QString>>> *updatedMetaData){
//    QList<QList<QString>> newValue = updatedMetaData->at(0);
//    this->setMetaCacheData(&newValue);
//    QList<QList<QString>> newValue1 = updatedMetaData->at(1);
//    this->setMetaMessageData(&newValue1);
//    QList<QList<QString>> newValue2 = updatedMetaData->at(2);
//    this->setDefaultSortList(&newValue2);
    QList<QList<QString>> newCacheData;
    for(int item = 0; item < metaCacheData.size(); item++){
        for(int updatedItem = 0; updatedItem < updatedMetaData->at(0).size(); updatedItem++){
            if(metaCacheData.at(item).at(0).compare(updatedMetaData->at(0).at(updatedItem).at(0)) == 0){
//                QString newValue = updatedMetaData->at(0).at(updatedItem).at(1);
//                metaCacheData.at(item).replace(1,newValue);
                newCacheData.append(updatedMetaData->at(0).at(updatedItem));
            }
        }
    }
    this->setMetaCacheData(&newCacheData);
    QList<QList<QString>> newMessageData;
    for(int item = 0; item < metaMessageData.size(); item++){
        for(int updatedItem = 0; updatedItem < updatedMetaData->at(1).size(); updatedItem++){
            if(metaMessageData.at(item).at(0).compare(updatedMetaData->at(1).at(updatedItem).at(0)) == 0){
//                metaMessageData.at(item).replace(1,updatedMetaData->at(1).at(updatedItem).at(1));
                newMessageData.append(updatedMetaData->at(1).at(updatedItem));
            }
        }
    }
    this->setMetaMessageData(&newMessageData);
    QList<QList<QString>> newSortOrder;
    newSortOrder = updatedMetaData->at(2);
    this->setDefaultSortList(&newSortOrder);
    return QString("submitted");
}

void TableController::deleteRow(int row){
    QString rowHeader = this->tableView->verticalHeaderItem(row)->text();
    for(int i = 0; i < dataTable.getAllFields()->size(); i++){
        if(dataTable.getAllFields()->at(i).compare(rowHeader) == 0){
            dataTable.getRows()->removeAt(i);
            dataTable.removeField(i);
            qDebug() << "removed " + rowHeader;
            break;
        }
    }
    tableView->removeRow(row);
}

void TableController::deleteColumn(int column){
    QString columnHeader = this->tableView->horizontalHeaderItem(column)->text();
    for(int i = 0; i < dataTable.getAllAttributes()->size(); i++){
        if(dataTable.getAllAttributes()->at(i).compare(columnHeader) == 0){
            for(int j = 0; j < dataTable.getRows()->size(); j++){
                dataTable.removeAttribute(i);
                dataTable.getRows()->at(j)->getRowCells()->removeAt(i);
                qDebug() << "removed " + columnHeader + " from " + dataTable.getAllFields()->at(j);
            }
            break;
        }
    }
    tableView->removeColumn(column);
    CacheConfig *config = CacheConfig::getInstance();
    for(int field = 0; field < config->columnFieldList()->size(); field++){
        if(config->columnFieldList()->at(field)->displayName().compare(columnHeader) == 0){
            config->columnFieldList()->removeAt(field);
            qDebug() << "removed column from cache config.";
            break;
        }
    }
}

void TableController::updateTableModel(QWidget *sender){
    qDebug() << "updating table model";
//    QTableWidgetItem *tableItem = (QTableWidgetItem*)sender->property("tableItem").value<void*>();
//    qDebug() << static_cast<FieldType>(sender->property("type").toInt());
    int fieldType = static_cast<FieldType>(sender->property("type").toInt());
    QString errorString;
    if(fieldType == 0){
        QLineEdit *valueLineEdit = (QLineEdit*)sender;
        errorString = dataTable.updateTableModel(valueLineEdit->property("row").toInt(), valueLineEdit->property("column").toInt(), valueLineEdit->text());
    }else if(fieldType == 1){
        QComboBox *valueComboBox = (QComboBox*)sender;
        errorString = dataTable.updateTableModel(valueComboBox->property("row").toInt(), valueComboBox->property("column").toInt(), valueComboBox->currentText());
    }else if(fieldType == 2){
        QCheckBox *valueCheckBox = (QCheckBox*)sender;
        if(valueCheckBox->checkState() == Qt::PartiallyChecked){
            errorString = dataTable.updateTableModel(valueCheckBox->property("row").toInt(), valueCheckBox->property("column").toInt(), "(Not Applicable)");
        }else if(valueCheckBox->checkState() == Qt::Checked){
            errorString = dataTable.updateTableModel(valueCheckBox->property("row").toInt(), valueCheckBox->property("column").toInt(), "true");
        }else{
            errorString = dataTable.updateTableModel(valueCheckBox->property("row").toInt(), valueCheckBox->property("column").toInt(), "false");
        }
    }else if(fieldType == 3){
        QSpinBox *valueSpinBox = (QSpinBox*)sender;
        if(valueSpinBox->specialValueText().compare("") != 0 && valueSpinBox->value() == valueSpinBox->minimum()){
            errorString = dataTable.updateTableModel(valueSpinBox->property("row").toInt(), valueSpinBox->property("column").toInt(), QString("(Not Applicable)"));
        }else{
            errorString = dataTable.updateTableModel(valueSpinBox->property("row").toInt(), valueSpinBox->property("column").toInt(), QString::number(valueSpinBox->value()));
        }
    }
    if(errorString.compare("submitted") != 0){
        if(fieldType == 0){
            ((QLineEdit*)sender)->selectAll();
        }
        QMessageBox msg(QMessageBox::Warning, "Update Failed", errorString);
        msg.exec();
    }/*else if(returnArray->size() == 3){
        QMessageBox msg(QMessageBox::Warning, "Updated Successfully", "'" + returnArray->at(1) + "' of '" + returnArray->at(0) + "' updated to " + returnArray->at(2));
        msg.exec();
    }*/
}

//QString TableController::getColorCode(QString value, FieldInfo *field){

//}

void TableController::populateTableModel(XMLFileData *data)
{
    Table *table = this->getDataTable();
    CacheConfig *config = CacheConfig::getInstance();
    foreach(QString rowHeader , data->allFields)
    {
        table->addField(rowHeader);
    }
    foreach(QString columnHeader , data->allAttributes)
    {
        foreach(FieldInfo *field , *(config->columnFieldList())){
            if(field->name().compare(columnHeader) == 0){
                table->addAttribute(field->displayName());
            }
        }
    }
    for(int row=0; row<data->tableData.count(); row++)
    {
        for(int column=0; column<data->tableData.at(row).count(); column++)
        {
            QString attributeName = data->allAttributes.at(column);
            FieldInfo *attributeInfo = config->columnFieldList()->at(config->posForColumnName(attributeName));
            if(attributeInfo)
            {
                QString value = data->tableData.at(row).at(column);
                table->addCell(row,column,value,attributeInfo);
            }
            else
                qDebug() << "Error FieldInfo null" ;
        }

    }
    this->setMetaCacheData(&(data->metaCacheData));
    this->setMetaMessageData(&(data->metaMessageData));
    this->setDefaultSortList(&(data->defaultSortList));
}
void TableController::createTableView()
{
    this->tableView->disconnect();
    QIcon *warningIcon = MainWindow::mainWindow->getWarningIcon();
    Table *table = this->getDataTable();
    QTableWidget *tableWidget = this->getTableView();
    tableWidget->setProperty("tableController",qVariantFromValue((void*)this));
    tableWidget->verticalHeader()->setSectionsMovable(true);
    tableWidget->verticalHeader()->setDragEnabled(true);
    tableWidget->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setRowCount(table->getAllFields()->size());
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
//    tableWidget->setSelectionBehavior(QAbstractItemView::SelectColumns);
    QObject::connect(tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), MainWindow::mainWindow, SLOT(setSelectedColumn(int)));
    QObject::connect(tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)),  MainWindow::mainWindow, SLOT(setSelectedRow(int)));
    for ( QList<TableRow*>::iterator tableRow = table->getRows()->begin(); tableRow != table->getRows()->end(); ++tableRow)
    {
        QString errorString;
        int row = tableRow - table->getRows()->begin();
        for ( QList<TableCell*>::iterator rowCell = (*tableRow)->getRowCells()->begin(); rowCell != (*tableRow)->getRowCells()->end(); ++rowCell)
        {
            int column = rowCell - (*tableRow)->getRowCells()->begin();
            addCellToTableView(row, column, (*rowCell)->getValue(), (*rowCell)->getField(), tableWidget);
            if(table->getErrorsMap()->contains(*rowCell)){
                errorString.append(table->getErrorsMap()->find(*rowCell).value());
            }
        }
        if(errorString.length() > 0){
            QTableWidgetItem* warningItem = new QTableWidgetItem();
            warningItem->setToolTip(errorString);
            warningItem->setIcon(*warningIcon);
            tableWidget->setVerticalHeaderItem(row,warningItem);
        }
    }
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    tableWidget->setVerticalHeaderLabels(*(table->getAllFields()));
//    QObject::connect(this->tableView,SIGNAL(cellChanged(int,int)),MainWindow::mainWindow,SLOT(cellMoved(int,int)));
}

void TableController::addAttributeToTableView(FieldInfo *newField){
//    this->tableView->disconnect();
    QTableWidget *tableWidget = this->getTableView();
    Table *table = this->getDataTable();
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    int column = table->getAllAttributes()->count()-1;
    for(int row=0; row<table->getAllFields()->count(); row++){
        addCellToTableView(row, column, newField->defaultVal(), newField, tableWidget);
    }
}

void TableController::addFieldToTableView(int row){
//    this->tableView->disconnect();
    QTableWidget *tableWidget = this->getTableView();
    Table *table = this->getDataTable();
    tableWidget->setRowCount(table->getAllFields()->count());
    tableWidget->setVerticalHeaderLabels(*(table->getAllFields()));
    for(int column=0; column<table->getAllAttributes()->count(); column++){
        TableCell *cell = table->getRows()->at(row)->getRowCells()->at(column);
        addCellToTableView(row, column, cell->getValue(), cell->getField(), tableWidget);
    }
}

void TableController::addCellToTableView(int row, int column, QString value, FieldInfo *field, QTableWidget *tableWidget){
    QSignalMapper *signalMapper = new QSignalMapper(MainWindow::mainWindow);
    if(field->fieldType() == FieldType::DropDown){
        QComboBox *comboBox = new QComboBox(tableWidget);
        comboBox->setProperty("type",FieldType::DropDown);
        comboBox->setProperty("row",row);
        comboBox->setProperty("column",column);
        comboBox->setFocusPolicy(Qt::ClickFocus);
        int currentIndex = 0;
        int indexCounter = 0;
        QMap<QString,QString> *valueSet = field->dropDownValMap();
        foreach(QString key , valueSet->keys()){
            comboBox->addItem(key);
            if(key.compare(value) == 0){
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        if(field->nullable()){
            comboBox->addItem("");
            if(value.compare("") == 0){
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        if(!field->isMandetory()){
            comboBox->addItem("(Not Applicable)");
            if(value.compare("(Not Applicable)") == 0){
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        comboBox->setCurrentIndex(currentIndex);
        tableWidget->setCellWidget(row, column, comboBox);
        signalMapper->setMapping(comboBox,comboBox);
        QObject::connect(comboBox,SIGNAL(currentIndexChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }else if(field->fieldType() == FieldType::Bool){
        QWidget *pWidget = new QWidget(tableWidget);
        QHBoxLayout *layout = new QHBoxLayout(pWidget);
        QCheckBox *checkBox = new QCheckBox(pWidget);
        if(!field->isMandetory()){
            checkBox->setTristate(true);
        }
        checkBox->setProperty("type",FieldType::Bool);
        checkBox->setProperty("row",row);
        checkBox->setProperty("column",column);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(layout);
        if(value.compare(QString("true")) == 0)
            checkBox->setChecked(true);
        else if(value.compare(QString("(Not Applicable)")) == 0)
            checkBox->setCheckState(Qt::PartiallyChecked);
        tableWidget->setCellWidget(row, column, pWidget);
        signalMapper->setMapping(checkBox,checkBox);
        QObject::connect(checkBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }else if(field->fieldType() == FieldType::Integer){
        QSpinBox *spinBox = new QSpinBox(tableWidget);
        spinBox->setProperty("type",FieldType::Integer);
        spinBox->setProperty("row",row);
        spinBox->setProperty("column",column);
        spinBox->setMaximum(field->maxRange());
        spinBox->setMinimum(field->minRange());
        if(!field->isMandetory()){
            spinBox->setMinimum(field->minRange()-1);
            spinBox->setSpecialValueText("(Not Applicable)");
        }
        if(value.compare("(Not Applicable)") == 0){
            spinBox->setValue(field->minRange()-1);
        }else{
            spinBox->setValue(value.toInt());
        }
        tableWidget->setCellWidget(row, column,spinBox);
        signalMapper->setMapping(spinBox,spinBox);
        QObject::connect(spinBox,SIGNAL(valueChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }else if(field->fieldType() == FieldType::Text){
        QLineEdit *lineEdit = new QLineEdit(tableWidget);
        lineEdit->setProperty("type",FieldType::Text);
        lineEdit->setProperty("row",row);
        lineEdit->setProperty("column",column);
        lineEdit->setText(value);
        tableWidget->setCellWidget(row, column,lineEdit);
        signalMapper->setMapping(lineEdit,lineEdit);
        QObject::connect(lineEdit,SIGNAL(textChanged(QString)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }
}

QString TableController::addAttributeToModel(FieldInfo *newField){
    return this->getDataTable()->addAttributeToModel(newField);
}

QString TableController::addFieldToModel(QMap<QString,QString> *fieldData){
    return this->getDataTable()->addFieldToModel(fieldData);
}
