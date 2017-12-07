#include "AddFieldWindow.h"
#include "ui_AddFieldWindow.h"

AddFieldWindow::AddFieldWindow(QWidget *parent, QTableWidget *tableWidget) :
    QDialog(parent, Qt::Window ), ui(new Ui::AddFieldWindow) {
    ui->setupUi(this);
    this->tableWidget = tableWidget;
    this->populateAttributes();
}
AddFieldWindow::~AddFieldWindow() {
    delete ui;
}

QMap<QString,QString>* AddFieldWindow::getWindowData(){
    return &(this->windowData);
}

void AddFieldWindow::populateAttributes(){
    TableController *tableController = (TableController*)this->tableWidget->property("tableController").value<void*>();
    Table *table = tableController->getDataTable();
    QList<FieldInfo*> columnFields;
    foreach(QString attributeName, *(table->getAllAttributes()))
    {
        foreach(FieldInfo * item, *(tableController->getConfig()->columnFieldList()))
        {
            if(item->displayName().compare(attributeName) == 0)
            {
                columnFields.push_back(item);
            }
        }
    }
    foreach(FieldInfo *item, columnFields)
    {
        QLabel *label;
        if(item->fieldType() != FieldType::Bool)
        {
            label = new QLabel(item->displayName(), this);

        }
        switch (item->fieldType())
        {
        case FieldType::Text:
        {
            QLineEdit *lineEdit= new QLineEdit(item->defaultVal(), this);
            lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            lineEdit->setObjectName(item->name());
            ui->formLayout->addRow(label,lineEdit);
        }

              break;
        case FieldType::DropDown:
        {
            QComboBox *combo = new QComboBox(this);
            combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            for (QMap<QString, QString>::iterator dropItem = item->dropDownValMap()->begin(); dropItem != item->dropDownValMap()->end();++dropItem)
            {
                combo->addItem(dropItem.key(), dropItem.value());
            }
            if (item->nullable())
                combo->addItem(QString("(Empty)"));
            if (!item->isMandetory())
                combo->addItem(QString("(Not Applicable)"));
            combo->setCurrentText(item->defaultVal());
            combo->setObjectName(item->name());
            ui->formLayout->addRow(label,combo);

        }
            break;
        case FieldType::Integer:
        {
            QSpinBox *spinBox = new QSpinBox(this);
            spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            spinBox->setMaximum(item->maxRange());
            spinBox->setMinimum(item->minRange());
            if(!item->isMandetory()){
                spinBox->setMinimum(item->minRange()-1);
                spinBox->setSpecialValueText("(Not Applicable)");
            }
            spinBox->setValue(item->defaultVal().toInt());
            spinBox->setObjectName(item->name());
            ui->formLayout->addRow(label,spinBox);
        }
            break;
        case FieldType::Invalid:
        default:
            break;
        }
    }

    foreach(FieldInfo *item, columnFields)
    {
        if(item->fieldType() == FieldType::Bool)
        {
            QCheckBox *checkBox = new QCheckBox(this);
            checkBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            if(!item->isMandetory()){
                checkBox->setTristate(true);
            }
            checkBox->setObjectName(item->name());
            checkBox->setChecked(item->defaultVal() == "true"? true : false);

            QWidget *widget = new QWidget(this);
            widget->setObjectName(QStringLiteral("widget"));
            QFormLayout *formLayout1 = new QFormLayout(widget);
            formLayout1->setObjectName(QStringLiteral("formLayout"));
            formLayout1->setContentsMargins(0, 0, 0, 0);
            QLabel *label = new QLabel(widget);
            label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            label->setObjectName(item->name() + QStringLiteral("Label"));
            label->setText(item->displayName());

            formLayout1->setWidget(0, QFormLayout::FieldRole, label);

            formLayout1->setWidget(0, QFormLayout::LabelRole, checkBox);

            widget->setLayout(formLayout1);

            ui->formLayout->setWidget(ui->formLayout->count(), QFormLayout::FieldRole, widget);
        }
    }
}

void AddFieldWindow::saveData(){
    TableController *tableController = (TableController*)this->tableWidget->property("tableController").value<void*>();
    if(tableController){
        foreach(FieldInfo * item, *(tableController->getConfig()->columnFieldList()))
        {
            switch (item->fieldType())
            {
                case FieldType::Text:
                {
                    QLineEdit *lineEdit = ui->scrollAreaWidgetContents->findChild<QLineEdit*>(item->name());
                    if(lineEdit){
                        windowData.insert(item->name(),lineEdit->text());
                    }
                }
                break;
                case FieldType::DropDown:
                {
                    QComboBox *combo = ui->scrollAreaWidgetContents->findChild<QComboBox*>(item->name());
                    if(combo){
                        windowData.insert(item->name(),combo->currentText());
                    }
                }
                break;
                case FieldType::Bool:
                {
                    QCheckBox *checkBox = ui->scrollAreaWidgetContents->findChild<QCheckBox*>(item->name());
                    if(checkBox){
                        if(checkBox->checkState() == Qt::Checked){
                            windowData.insert(item->name(),"true");
                        }else if(checkBox->checkState() == Qt::PartiallyChecked){
                            windowData.insert(item->name(),"(Not Applicable)");
                        }else{
                            windowData.insert(item->name(),"false");
                        }
                    }
                }
                break;
                case FieldType::Integer:
                {
                    QSpinBox *spinBox = ui->scrollAreaWidgetContents->findChild<QSpinBox*>(item->name());
                    if(spinBox){
                        if(spinBox->specialValueText().compare("") != 0 && spinBox->value() == spinBox->minimum()){
                            windowData.insert(item->name(),"(Not Applicable)");
                        }else{
                            windowData.insert(item->name(),spinBox->text());
                        }

                    }
                }
                break;
            }
        }
        QMap<QString,QString> *windowData = this->getWindowData();
        QString returnValue = tableController->addFieldToModel(windowData);
        if(returnValue.compare("submitted") == 0){
            qDebug() << "Successfully added the field to the model";
            tableController->addFieldToTableView(tableController->getDataTable()->getAllFields()->count()-1);
            accept();
        }else {
            this->windowData.clear();
            QMessageBox msg(QMessageBox::Warning, "Failed to submit form", returnValue);
            msg.exec();
        }

    }
}
