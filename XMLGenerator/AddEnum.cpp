#include "AddEnum.h"
#include "ui_AddEnum.h"
#include <TableController.h>

AddEnum::AddEnum(QWidget *parent, QString *filePath) :
    QDialog(parent),
    ui(new Ui::AddEnum)
{
    this->filePath = filePath;
    this->signalMapper = new QSignalMapper(this);
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,162);
    ui->tableWidget->setColumnWidth(1,162);
    ui->tableWidget->setColumnWidth(2,40);
    newValueID = 0;
}

AddEnum::~AddEnum()
{
    delete ui;
}

void AddEnum::makeNecessary()
{
//    ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
}

void AddEnum::makeUnclosable()
{
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
}

void AddEnum::setEnumName(QString enumName){
    ui->nameLineEdit->setText(enumName);
    ui->nameLineEdit->setEnabled(false);
}

void AddEnum::addValue(){
//    ui->formLayout->itemAt()
    QString valueName = ui->groupBox_2->findChild<QLineEdit*>("valueNameLineEdit")->text();
    QString value = ui->groupBox_2->findChild<QLineEdit*>("valueLineEdit")->text();
    QTableWidget *tableWidget = ui->tableWidget;
    QString errorString;
    if(valueName.compare("") == 0){
        errorString.append("Name of the value cannot be empty. \n");
    }
    if(value.compare("") == 0){
        errorString.append("Value cannot be empty. \n");
    }
    for(int row = 0; row<tableWidget->rowCount(); row++){
        if(tableWidget->item(row,0)->text().compare(valueName) == 0){
            errorString.append("Value name '"+ valueName + "' already exists \n");
        }
        if(tableWidget->item(row,1)->text().compare(value) == 0){
            errorString.append("Value '"+ value + "' already exists \n");
        }
    }
    if(errorString.length() == 0){
        int newRowIndex = tableWidget->rowCount();
        tableWidget->setRowCount(newRowIndex+1);
        ui->groupBox_2->findChild<QLineEdit*>("valueNameLineEdit")->clear();
        ui->groupBox_2->findChild<QLineEdit*>("valueLineEdit")->clear();
        tableWidget->setItem(newRowIndex,0,new QTableWidgetItem(valueName));
        tableWidget->setItem(newRowIndex,1,new QTableWidgetItem(value));
        QPushButton *removeBtn = new QPushButton("", tableWidget);
        QIcon removeIcon(QIcon::fromTheme(QStringLiteral(":/icons/Delete.png")));
        removeBtn->setIcon(removeIcon);
        removeBtn->setToolTip("Remove");
        removeBtn->setFlat(true);
        removeBtn->setProperty("id",newValueID);
        signalMapper->setMapping(removeBtn,newValueID);
        QObject::connect(removeBtn,SIGNAL(clicked()),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(removeValue(int)));
    //    connect(ui->tableWidget, SIGNAL(clicked(const QModelIndex &)), this, SLOT(removeValue(const QModelIndex &)));
        tableWidget->setCellWidget(newRowIndex,2,removeBtn);
        newValueID++;
    }else{
        QMessageBox msg(QMessageBox::Warning, "Failed to add value", errorString);
        msg.exec();
    }
//    tableWidget->resizeColumnsToContents();
}

void AddEnum::removeValue(int id){
    QTableWidget *tableWidget = ui->tableWidget;
    for(int row=0; row < tableWidget->rowCount(); row++)
    {
        if(tableWidget->cellWidget(row,2)->property("id") == id)
        {
            tableWidget->model()->removeRow(row);
            break;
        }
    }
//    tableWidget->resizeColumnsToContents();
}

void AddEnum::saveEnum(){
    QString errorString;
    CacheConfig *config = MainWindow::mainWindow->getTableControllerMap()->find(*filePath).value()->getConfig();
    FieldInfo *enumField = config->columnFieldList()->at(config->posForColumnName("enumName"));
    QMap<QString,QString> *enumValues = enumField->dropDownValMap();
    QString enumName = ui->nameLineEdit->text();
    if(enumName.compare("") == 0){
        errorString.append("Enum name cannot be empty.\n");
    }
    if(ui->tableWidget->rowCount() == 0){
        errorString.append("Add at least one name-value pair.\n");
    }
    QMap<QString,QString>::iterator enumValuesIterator = enumValues->begin();
    if(enumValuesIterator != enumValues->end()){
        if(enumValuesIterator.key().compare(enumName) == 0){
            errorString.append("Enum name already exists!");
        }
        enumValuesIterator++;
    }
    if(errorString.size() > 0){
        QMessageBox msg(QMessageBox::Warning, "Failed to submit form", errorString);
        msg.exec();
    }else{
        Enum *newEnum = new Enum(enumName);
        for(int row = 0; row < ui->tableWidget->rowCount(); row++){
            QString value = ui->tableWidget->item(row,1)->text();
            QString name = ui->tableWidget->item(row,0)->text();
            newEnum->addValue(value, name);
        }
        MainWindow::mainWindow->saveEnum(newEnum,filePath);
        accept();
    }
}
