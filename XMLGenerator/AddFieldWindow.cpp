#include "AddFieldWindow.h"
#include "ui_AddFieldWindow.h"

AddFieldWindow::AddFieldWindow(QWidget *parent,QList<QString> *attributesList,CacheConfig *config) : QDialog(parent, Qt::Window ), ui(new Ui::AddFieldWindow) {
    ui->setupUi(this);
    this->attributesList = *attributesList;
    this->config = *config;
    this->populateAttributes();
}
AddFieldWindow::~AddFieldWindow() {
    delete ui;
}

QMap<QString,QString>* AddFieldWindow::getWindowData(){
    return &(this->windowData);
}

void AddFieldWindow::populateAttributes(){
    int noOfAttributes = this->attributesList.count();
    for(int i = 0; i<noOfAttributes; i++){
        QString attributeName = attributesList.at(i);
        int configColumnPos = this->config.posForColumnName(attributeName);
        FieldInfo attributeInfo = config.columnFieldList()->at(configColumnPos);
        if(attributeInfo.fieldType() == FieldType::DropDown){
            QComboBox *comboBox = new QComboBox();
            comboBox->setProperty("variableName",attributeName);
            QList<QString> valueSpace = attributeInfo.dropDownValMap()->keys();
            for(int j=0; j< valueSpace->count(); j++){
                comboBox->addItem(valueSpace.at(j));
            }
            ui->formLayout->addRow(attributeName, comboBox);
        }else if(attributeInfo.fieldType() == FieldType::Integer){
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setProperty("variableName",attributeName);
            ui->formLayout->addRow(attributeName, spinBox);
        }else if(attributeInfo.fieldType() == FieldType::Text){
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setProperty("variableName",attributeName);
            ui->formLayout->addRow(attributeName, lineEdit);
        }else if(attributeInfo.fieldType() == FieldType::Bool) {
            QCheckBox *checkBox = new QCheckBox(this);
            checkBox->setProperty("variableName",attributeName);
            ui->formLayout->addRow(attributeName, checkBox);
        }
    }
}

void AddFieldWindow::saveData(){
    QList<QLineEdit*> allLineEdits = ui->scrollAreaWidgetContents->findChildren<QLineEdit*>();
    QList<QComboBox*> allComboBoxes = ui->scrollAreaWidgetContents->findChildren<QComboBox*>();
    QList<QSpinBox*> allSpinBoxes = ui->scrollAreaWidgetContents->findChildren<QSpinBox*>();
    QList<QCheckBox*> allCheckBoxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
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
}
