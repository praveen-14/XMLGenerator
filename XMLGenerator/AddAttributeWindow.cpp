#include "AddAttributeWindow.h"
#include "ui_AddAttributeWindow.h"
#include <QDebug>

AddAttributeWindow::AddAttributeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAttributeWindow)
{
    ui->setupUi(this);
    this->initializeWindow();
}

AddAttributeWindow::~AddAttributeWindow()
{
    delete ui;
}

QMap<QString,QString>* AddAttributeWindow::getWindowData(){
    return &(this->windowData);
}

void AddAttributeWindow::initializeWindow(){
    QLineEdit* attributeName = new QLineEdit(this);
    attributeName->setProperty("variableName","attributeName");
    ui->formLayout->addRow("Attribute Name", attributeName);
    QLineEdit* valuesSet = new QLineEdit(this);
    valuesSet->setPlaceholderText("values separated by commas. Example : value1(default),value2,value3..");
    valuesSet->setProperty("variableName","valueSpace");
    ui->formLayout->addRow("Value Space(optional)", valuesSet);
}

void AddAttributeWindow::saveData(){
    QList<QComboBox*> allComboBoxes = ui->scrollAreaWidgetContents->findChildren<QComboBox*>();
    QList<QLineEdit*> allLineEdits = ui->scrollAreaWidgetContents->findChildren<QLineEdit*>();
    if (allLineEdits.count() > 0){
        QList<QLineEdit*>::iterator iterator = allLineEdits.begin();
        while(iterator != allLineEdits.end()){
            windowData.insert((*iterator)->property("variableName").toString(),(*iterator)->text());
            iterator++;
        }
    }
    if (allComboBoxes.count() > 0){
        QList<QComboBox*>::iterator iterator = allComboBoxes.begin();
        while(iterator != allComboBoxes.end()){
            windowData.insert((*iterator)->property("variableName").toString(),(*iterator)->currentText());
            iterator++;
        }
    }
//    QList<QCheckBox*> allCheckBoxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
//    if (allCheckBoxes.count() > 0){
//        QList<QCheckBox*>::iterator iterator = allCheckBoxes.begin();
//        while(iterator != allCheckBoxes.end()){
//            if((*iterator)->checkState() == Qt::Checked){
//                windowData.insert("nullable",Qt);
//            }
//            iterator++;
//        }
//    }
}
