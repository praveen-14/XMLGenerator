#include "AddFieldWindow.h"
#include "ui_AddFieldWindow.h"

AddFieldWindow::AddFieldWindow(QWidget *parent) : QDialog(parent, Qt::Window ), ui(new Ui::AddFieldWindow) {
    ui->setupUi(this);
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    this->populateAttributes(MainWindow::mainWindow->getTableModel((QTableWidget*)(tabWidget->widget(tabWidget->currentIndex()))));
}
AddFieldWindow::~AddFieldWindow() {
    delete ui;
}

QMap<QString,QString>* AddFieldWindow::getWindowData(){
    return &(this->windowData);
}

void AddFieldWindow::populateAttributes(Table* table){
    QMap<int, QList<QString>> fieldValues = MainWindow::mainWindow->getFileController()->loadFieldValues(*(table->getAllAttributes()));
    for(int i = 0; i<table->getAllAttributes()->length(); i++){
        if(fieldValues.contains(i)){
            QComboBox *comboBox = new QComboBox();
            comboBox->setProperty("variableName",table->getAllAttributes()->at(i));
            for(int j=0; j< fieldValues.value(i).length(); j++){
                comboBox->addItem(fieldValues.value(i).value(j));
            }
            ui->formLayout->addRow(table->getAllAttributes()->at(i), comboBox);
        }else{
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setProperty("variableName",table->getAllAttributes()->at(i));
            ui->formLayout->addRow(table->getAllAttributes()->at(i), lineEdit);
        }
    }
}

void AddFieldWindow::saveData(){
    QList<QLineEdit*> allLineEdits = ui->formLayout->findChildren<QLineEdit*>();
    QList<QComboBox*> allComboBoxes = ui->formLayout->findChildren<QComboBox*>();
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
