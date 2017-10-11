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
    AddFieldWindow *addFieldWindow = new AddFieldWindow(this);
    addFieldWindow->setModal(true);
    addFieldWindow->exec();
    /*
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    Table *table = this->getTableModel(tableWidget);
    QMap<int, QList<QString>> fieldValues = MainWindow::mainWindow->getFileController()->loadFieldValues(table->getAllAttributes());
    QMap<QString,QString> *windowData = addFieldWindow->getWindowData();
    table->addField(windowData->find("displayName").value());
    for(int column=0;column<table->getAllAttributes()->count();column++){
        QList<QString> valueSet = fieldValues.find(column).value();
        table->addCell(table->getAllFields()->count()-1,column,windowData->find(table->getAllAttributes().at(colum)).value(),valueSet);
    }
//    TableController *tableController = this->getTableController(tableWidget->property("filePath").toString());
//    tableController->addFieldToTableView(attributeName,valueSet);
*/
}

void MainWindow::addAttribute(){
//    AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(this);
//    addAttributeWindow->setModal(true);
//    addAttributeWindow->exec();
//    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
//    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
//    TableController *tableController = (TableController*)tableWidget->property("tableModel").value<void*>();
//    QMap<QString,QString> *windowData = addAttributeWindow->getWindowData();
//    tableController->addAttributeToModel(windowData);
//    tableController->addAttributeToTableView(attributeName,valueSet);
}

void MainWindow::addTableController(XMLFileData xmlData)
{
    QString fileName = xmlData.fileName;
    QString filePath = xmlData.filePath;
    if(MainWindow::mainWindow->tableControllerMap.contains(filePath)){
        QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
        tabWidget->setCurrentIndex(tabWidget->indexOf(MainWindow::mainWindow->getTableController(filePath)->getTableView()));
    }else{
        TableController *tableController = new TableController(xmlData.filePath);
//        tableController->setFilePath(xmlData.filePath);
        MainWindow::mainWindow->tableControllerMap.insert(filePath,tableController);
        tableController->populateTableModel(tableController->getDataTable(),xmlData);
        QTableWidget *tableWidget = tableController->createTableView(fileName);
        QTabWidget *tabWidget = ui->frame->findChild<QTabWidget*>("tabWidget");
        tabWidget->addTab(tableWidget,fileName);
//        tableController->populateTableView(tableController->getDataTable(),MainWindow::mainWindow->getMainUI()->tableWidget);
    }
}

void MainWindow::loadFile()
{
    XMLFileData XMLData = MainWindow::mainWindow->getFileController()->loadFile();
    QMap<int, QList<QString>> fieldValues = MainWindow::mainWindow->getFileController()->loadFieldValues(XMLData.allAttributes);
    QMap<QString, QList<QString>>::iterator iterator = XMLData.valueSets.begin();
    while (iterator != XMLData.valueSets.end()) {
        if(fieldValues.find(XMLData.allAttributes.indexOf(iterator.key())) != fieldValues.end()){
            foreach(QString value , iterator.value()){
                if(!fieldValues.find(XMLData.allAttributes.indexOf(iterator.key())).value().contains(value)){
                    QList<QString> valueInList;
                    valueInList.append(value);
                    this->getFileController()->updateFieldValues(iterator.key(),valueInList);
                }
            }
        }
        iterator++;
    }
    this->addTableController(XMLData);
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
