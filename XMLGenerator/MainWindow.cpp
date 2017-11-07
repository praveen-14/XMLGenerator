#include <MainWindow.h>
#include <ui_MainWindow.h>
#include <TableController.h>
#include <AddFieldWindow.h>
#include <AddAttributeWindow.h>
#include <UpdateMetaDataWindow.h>

MainWindow* MainWindow::mainWindow = nullptr;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    MainWindow::mainWindow = this;
    this->warningIcon = *(new QIcon("C:/Users/praveense/Downloads/warning-512.png"));
    CacheConfig::getInstance()->init();
    MainWindow::mainWindow->ui->setupUi(MainWindow::mainWindow);
    QGridLayout *grid = new QGridLayout(MainWindow::mainWindow->ui->frame);
    QTabWidget *tabWidget = new QTabWidget(MainWindow::mainWindow->ui->frame);
    tabWidget->setTabsClosable(true);
    connect(tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged()));
    grid->addWidget(tabWidget);
    grid->setObjectName("gridLayout");
    tabWidget->setObjectName("tabWidget");
    MainWindow::mainWindow->ui->frame->setLayout(grid);
}

MainWindow::~MainWindow()
{
    qDebug() <<  "deleted";
    delete ui;
//    delete mainWindow;
//    delete &MainWindow::mainWindow;
}

void MainWindow::closeTab(int index){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(index);
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm", "Close file '" + tabWidget->tabText(index) + "'? All unsaved data will be lost.", QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        QMap<QString,TableController*>::iterator tableControllerIterator = this->getTableControllerMap()->begin();
        while(tableControllerIterator != this->getTableControllerMap()->end()){
            if(tableControllerIterator.value() == tableController){
                tableControllerMap.remove(tableControllerIterator.key());
                break;
            }
            tableControllerIterator++;
        }
        tabWidget->removeTab(index);
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}

void MainWindow::updateMetaData(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    UpdateMetaDataWindow *updateMetaDataWindow = new UpdateMetaDataWindow(this,tableController->getMetaCacheData(),tableController->getMetaMessageData(), tableController->getDefaultSortList(), tableWidget);
    updateMetaDataWindow->setModal(true);
    updateMetaDataWindow->exec();
}

void MainWindow::save()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    XMLFileData xmlData;
    xmlData.fileName = tabWidget->tabText(tabWidget->currentIndex());
    xmlData.filePath = tableControllerMap.key(tableController);
    QList<FieldInfo*> fields;
    CacheConfig *config = CacheConfig::getInstance();
    for(int row = 0; row < tableWidget->rowCount(); row++){
        QList<QString> testList;
        xmlData.tableData.push_back(testList);
    }
    for(int row = 0; row < tableWidget->rowCount(); row++){
//        qDebug() << row;
//        qDebug() << tableWidget->visualRow(row);
//        qDebug() << "-----";
        xmlData.allFields.push_back(tableWidget->verticalHeaderItem(row)->text());
        QList<QString> fieldData;
        for(int column = 0; column < tableWidget->columnCount(); column++){
            if(row == 0){
                for(int i = 0; i < config->columnFieldList()->size(); i++){
                    if(config->columnFieldList()->at(i)->displayName().compare(tableWidget->horizontalHeaderItem(column)->text()) == 0){
                        fields.push_back(config->columnFieldList()->at(i));
                        break;
                    }
                }
                xmlData.allAttributes.push_back(fields.back()->name());
            }
            if(fields.at(column)->fieldType() == FieldType::Text){
                fieldData.append(((QLineEdit*)(tableWidget->cellWidget(row,column)))->text());
            }
            if(fields.at(column)->fieldType() == FieldType::DropDown){
                QString value = ((QComboBox*)(tableWidget->cellWidget(row,column)))->currentText();
                if(value.compare(QString("(Not Applicable)")) == 0){
                    fieldData.append(value);
                }else{
                    QString resolvedValue = fields.at(column)->dropDownValMap()->find(value).value();
                    fieldData.append(resolvedValue);
                }

            }
            if(fields.at(column)->fieldType() == FieldType::Bool){
                if(tableWidget->cellWidget(row,column)->findChild<QCheckBox*>()->checkState() == Qt::Checked){
                    fieldData.append("true");
                }else if(tableWidget->cellWidget(row,column)->findChild<QCheckBox*>()->checkState() == Qt::PartiallyChecked){
                    fieldData.append("(Not Applicable)");
                }else{
                    fieldData.append("false");
                }
            }
            if(fields.at(column)->fieldType() == FieldType::Integer){
                int value = ((QSpinBox*)(tableWidget->cellWidget(row,column)))->value();
                if(((QSpinBox*)(tableWidget->cellWidget(row,column)))->specialValueText().compare("") != 0 && ((QSpinBox*)(tableWidget->cellWidget(row,column)))->minimum() == value){
                    fieldData.append("(Not Applicable)");
                }else{
                    fieldData.append(QString::number(value));
                }

            }
        }
        xmlData.tableData.replace(tableWidget->visualRow(row),fieldData);
    }
    xmlData.metaCacheData = *(tableController->getMetaCacheData());
    xmlData.metaMessageData = *(tableController->getMetaMessageData());
    xmlData.defaultSortList = *(tableController->getDefaultSortList());
    fileController.save(xmlData);
}

void MainWindow::tabChanged(){
    ui->deleteColumnButton->setEnabled(false);
    ui->deleteRowButton->setEnabled(false);
}

void MainWindow::setSelectedRow(int row){
    ui->deleteRowButton->setEnabled(true);
    ui->deleteColumnButton->setEnabled(false);
    selectedRow = row;
}

void MainWindow::setSelectedColumn(int column){
    ui->deleteColumnButton->setEnabled(true);
    ui->deleteRowButton->setEnabled(false);
    selectedColumn = column;
}

void MainWindow::deleteRow(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm", "Remove '" + tableWidget->verticalHeaderItem(selectedRow)->text() + "' from table?", QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        tableController->deleteRow(selectedRow);
        ui->deleteRowButton->setEnabled(false);
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}

void MainWindow::deleteColumn(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm", "Remove '" + tableWidget->horizontalHeaderItem(selectedColumn)->text() + "' from table?" , QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        tableController->deleteColumn(selectedColumn);
        ui->deleteColumnButton->setEnabled(false);
    } else {
        qDebug() << "Yes was *not* clicked";
    }
}

QIcon* MainWindow::getWarningIcon(){
    return &(this->warningIcon);
}

FileController* MainWindow::getFileController(){
    return &(this->fileController);
}

QMap<QString, TableController*>* MainWindow::getTableControllerMap(){
    return &tableControllerMap;
}


TableController* MainWindow::getTableController(QString key)
{
    QMap<QString, TableController*>::iterator iterator = this->getTableControllerMap()->find(key);
    return iterator.value();
}

void MainWindow::addField(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    AddFieldWindow *addFieldWindow = new AddFieldWindow(this,tableWidget);
    addFieldWindow->setModal(true);
    addFieldWindow->exec();
}

void MainWindow::addAttribute(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(this,tableWidget);
    addAttributeWindow->setModal(true);
    addAttributeWindow->exec();
//    FieldInfo *newFieldInfo = addAttributeWindow->getNewField();
//    if(tableController ){
//        if(newFieldInfo->name().compare("") != 0){
//            tableController->addAttributeToModel(newFieldInfo);
//            tableController->addAttributeToTableView(newFieldInfo);
//        }
//    }
//    CacheConfig::getInstance()->addColumnField(newFieldInfo);
}

void MainWindow::addTableController(XMLFileData *xmlData)
{
    QString fileName = xmlData->fileName;
    QString filePath = xmlData->filePath;
    QTabWidget *tabWidget = ui->frame->findChild<QTabWidget*>("tabWidget");
    TableController *tableController = new TableController(tabWidget);
    MainWindow::mainWindow->tableControllerMap.insert(filePath,tableController);
    tableController->populateTableModel(xmlData);
    tableController->createTableView();
    QTableWidget *tableWidget = tableController->getTableView();
    if(tabWidget && tableController){
        tabWidget->insertTab(tabWidget->count(),tableWidget,QString(fileName));
        tabWidget->setCurrentIndex(tabWidget->indexOf(tableWidget));
    }
}

void MainWindow::loadFile()
{
    XMLFileData *XMLData = MainWindow::mainWindow->getFileController()->loadFile();
    if(XMLData){
        ui->addAttributeButton->setEnabled(true);
        ui->addFieldButton->setEnabled(true);
        ui->updateMetaDataButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        QString filePath = XMLData->filePath;
        if(MainWindow::mainWindow->tableControllerMap.contains(filePath)){
            QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
            tabWidget->setCurrentIndex(tabWidget->indexOf(MainWindow::mainWindow->getTableController(filePath)->getTableView()));
        }else{
            this->addTableController(XMLData);
        }
    }
}

Table* MainWindow::getTableModel(QTableWidget *tableWidget){
    return this->getTableControllerMap()->find(tableWidget->property("filePath").toString()).value()->getDataTable();
}
