#include <MainWindow.h>
#include <ui_MainWindow.h>
#include <TableController.h>
#include <AddFieldWindow.h>
#include <AddAttributeWindow.h>

MainWindow* MainWindow::mainWindow = nullptr;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    MainWindow::mainWindow = this;
    cacheConfig->init();
    MainWindow::mainWindow->getMainUI()->setupUi(MainWindow::mainWindow);
    QGridLayout *grid = new QGridLayout(MainWindow::mainWindow->getMainUI()->frame);
    QTabWidget *tabWidget = new QTabWidget(MainWindow::mainWindow->getMainUI()->frame);
    grid->addWidget(tabWidget);
    grid->setObjectName("gridLayout");
    tabWidget->setObjectName("tabWidget");
    MainWindow::mainWindow->getMainUI()->frame->setLayout(grid);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete &MainWindow::mainWindow;
}



TableController* MainWindow::getTableController(QString key)
{
    QMap<QString, TableController*>::iterator iterator = this->getTableControllerMap()->find(key);
    return iterator.value();
//    while (iterator != this->getTableControllerMap()->end() && iterator.key() == key) {
//        return &iterator;
//        ++i;
//    }
}


void MainWindow::syncTableChanges(){
//    this->clearAllProperties();
//    this->clearXMLData();
//    for(int i = 0; i < ui->tableWidget->model()->columnCount(); i++)
//    {
//        QList<QString> attributeData;
//        this->addToProperties(ui->tableWidget->model()->headerData(i, Qt::Horizontal).toString());
//        for(int j = 0; j < ui->tableWidget->model()->rowCount(); j++)
//        {
//            this->addToXMLData(ui->tableWidget->model()->headerData(i, Qt::Horizontal).toString(),ui->tableWidget->item(j,i)->text());
////            XMLData[ui->tableWidget->model()->headerData(i, Qt::Horizontal).toString()] << ui->tableWidget->item(j,i)->text();
//        }
//    }
//    qDebug() << this->getAllProperties();
}

void MainWindow::addField(){
//    this->syncTableChanges();
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    QList<FieldInfo*> *fieldValues = cacheConfig->columnFieldList();
    AddFieldWindow *addFieldWindow = new AddFieldWindow(this,tableController->getDataTable()->getAllAttributes(),fieldValues);
    addFieldWindow->setModal(true);
    addFieldWindow->exec();
    QMap<QString,QString> *windowData = addFieldWindow->getWindowData();
    if(tableController){
        QMap<int, QList<QString>> *fieldValues = this->getFileController()->loadFieldValues(tableController->getDataTable()->getAllAttributes());
        QMap<QString,QString>* validatedField = tableController->addFieldToModel(windowData,fieldValues);
        QString fieldName = validatedField->find("displayName").value();
        tableController->addFieldToTableView(&fieldName,validatedField,fieldValues);
    }
//    TableController *tableController = this->getTableController(tableWidget->property("filePath").toString());
//    tableController->addFieldToTableView(attributeName,valueSet);
}

void MainWindow::addAttribute(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(this);
    addAttributeWindow->setModal(true);
    addAttributeWindow->exec();
    QMap<QString,QString> *windowData = addAttributeWindow->getWindowData();
    if(tableController){
        QMap<QString,QList<QString>>* validatedAttribute = tableController->addAttributeToModel(windowData);
        QString attributeName = validatedAttribute->begin().key();
        QList<QString> validatedValues = validatedAttribute->begin().value();
        if(validatedValues.count()>0){
            this->fileController.updateFieldValues(attributeName,validatedValues);
        }
        tableController->addAttributeToTableView(attributeName,validatedValues);
    }
}

void MainWindow::addTableController(XMLFileData *xmlData)
{
    QString fileName = xmlData->fileName;
    QString filePath = xmlData->filePath;
    TableController *tableController = new TableController();
//      tableController->setFilePath(xmlData.filePath);
    MainWindow::mainWindow->tableControllerMap.insert(filePath,tableController);
    tableController->populateTableModel(xmlData,cacheConfig->columnFieldList());
    QTableWidget *tableWidget = tableController->createTableView(fileName);
    QTabWidget *tabWidget = ui->frame->findChild<QTabWidget*>("tabWidget");
    tabWidget->addTab(tableWidget,fileName);
    tabWidget->setCurrentIndex(tabWidget->indexOf(tableWidget));
//      tableController->populateTableView(tableController->getDataTable(),MainWindow::mainWindow->getMainUI()->tableWidget);
}

void MainWindow::loadFile()
{
    XMLFileData *XMLData = MainWindow::mainWindow->getFileController()->loadFile();
    if(XMLData){
        ui->addAttributeButton->setEnabled(true);
        ui->addFieldButton->setEnabled(true);
        QString fileName = XMLData->fileName;
        QString filePath = XMLData->filePath;
        if(MainWindow::mainWindow->tableControllerMap.contains(filePath)){
            QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
            tabWidget->setCurrentIndex(tabWidget->indexOf(MainWindow::mainWindow->getTableController(filePath)->getTableView()));
        }else{
            this->addTableController(XMLData);
        }
    }
//    QMap<int, QList<QString>> fieldValues = MainWindow::mainWindow->getFileController()->loadFieldValues(XMLData.allAttributes);
//    QMap<QString, QList<QString>>::iterator iterator = XMLData.valueSets.begin();
//    while (iterator != XMLData.valueSets.end()) {
//        if(fieldValues.find(XMLData.allAttributes.indexOf(iterator.key())) != fieldValues.end()){
//            foreach(QString value , iterator.value()){
//                if(!fieldValues.find(XMLData.allAttributes.indexOf(iterator.key())).value().contains(value)){
//                    QList<QString> valueInList;
//                    valueInList.append(value);
//                    this->getFileController()->updateFieldValues(iterator.key(),valueInList);
//                }
//            }
//        }
//        iterator++;
//    }
}

Table* MainWindow::getTableModel(QTableWidget *tableWidget){
    return this->getTableControllerMap()->find(tableWidget->property("filePath").toString()).value()->getDataTable();
//    QMap<QString, TableController*>::iterator iterator = MainWindow::mainWindow->getTableControllerMap()->begin();
//    while (iterator != MainWindow::mainWindow->getTableControllerMap()->end()) {
//        if((*iterator)->getTableView() == tableWidget){
//            return (*iterator)->getDataTable();
//        }
//        iterator++;
//    }
//    return nullptr;
}
