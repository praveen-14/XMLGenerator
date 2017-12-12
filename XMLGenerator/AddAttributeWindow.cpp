#include "AddAttributeWindow.h"
#include "ui_AddAttributeWindow.h"
#include <QDebug>

AddAttributeWindow::AddAttributeWindow(QWidget *parent , QTableWidget *tableWidget) :
    QDialog(parent),
    ui(new Ui::AddAttributeWindow),newField(parent)
{
    this->signalMapper = new QSignalMapper(this);
    newValueID = 0;
    fieldData = nullptr;
    this->tableWidget = tableWidget;
    ui->setupUi(this);
    connect(ui->valueTypeComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(populateWindow(const QString&)));
    populateWindow("Text");
    this->setFixedSize(QSize(this->width(), this->height()));
}

AddAttributeWindow::~AddAttributeWindow()
{
    delete ui;
}

void AddAttributeWindow::setFieldName(QString fieldName)
{
    newField.setName(fieldName);
    ui->attributeNameLineEdit->setText(fieldName);
    ui->attributeNameLineEdit->setEnabled(false);
}

void AddAttributeWindow::setFieldData(QList<QString> *fieldData)
{
    this->fieldData = fieldData;
}

FieldInfo* AddAttributeWindow::getNewField()
{
    return &(this->newField);
}

void AddAttributeWindow::setDefaultMin(int min)
{
    QSpinBox *defaultSpin = ui->scrollAreaWidgetContents->findChild<QSpinBox*>("defaultValueSpinBox");
    QSpinBox *maxSpin = ui->scrollAreaWidgetContents->findChild<QSpinBox*>("maximumValueSpinBox");
    defaultSpin->setMinimum(min);
    if(maxSpin->text().toInt() < min)
    {
        maxSpin->setValue(min);
    }
}

void AddAttributeWindow::setDefaultMax(int max)
{
    QSpinBox *defaultSpin = ui->scrollAreaWidgetContents->findChild<QSpinBox*>("defaultValueSpinBox");
    QSpinBox *minSpin = ui->scrollAreaWidgetContents->findChild<QSpinBox*>("minimumValueSpinBox");
    defaultSpin->setMaximum(max);
    if(minSpin->text().toInt() > max)
    {
        minSpin->setValue(max);
    }
}

void AddAttributeWindow::addValue()
{
    QString valueName = ui->scrollAreaWidgetContents->findChild<QLineEdit*>("attributeValueNameLineEdit")->text();
    QString value = ui->scrollAreaWidgetContents->findChild<QLineEdit*>("attributeValueLineEdit")->text();
    QTableWidget *tableWidget = ui->scrollAreaWidgetContents->findChild<QTableWidget*>("tableWidget");
    QString errorString;
    if(valueName.compare("") == 0)
    {
        errorString.append("Name of the value cannot be empty. \n");
    }
    if(value.compare("") == 0)
    {
        errorString.append("Value cannot be empty. \n");
    }
    for(int row = 0; row<tableWidget->rowCount(); row++)
    {
        if(tableWidget->item(row,0)->text().compare(valueName) == 0)
        {
            errorString.append("Value name '"+ valueName + "' already exists \n");
        }
        if(tableWidget->item(row,1)->text().compare(value) == 0)
        {
            errorString.append("Value '"+ value + "' already exists \n");
        }
    }
    if(errorString.length() == 0)
    {
        int newRowIndex = tableWidget->rowCount();
        tableWidget->setRowCount(newRowIndex+1);
        ui->scrollAreaWidgetContents->findChild<QLineEdit*>("attributeValueNameLineEdit")->clear();
        ui->scrollAreaWidgetContents->findChild<QLineEdit*>("attributeValueLineEdit")->clear();
        ui->scrollAreaWidgetContents->findChild<QComboBox*>("defaultValueComboBox")->addItem(valueName);
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
        tableWidget->setCellWidget(newRowIndex,2,removeBtn);
        newValueID++;
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Failed to add value", errorString);
        msg.exec();
    }
}

void AddAttributeWindow::removeValue(int id)
{
    QTableWidget *tableWidget = ui->scrollAreaWidgetContents->findChild<QTableWidget*>("tableWidget");
    for(int row=0; row < tableWidget->rowCount(); row++)
    {
        QModelIndex index = tableWidget->model()->index(row, 0, QModelIndex());
        if(tableWidget->cellWidget(row,2)->property("id") == id)
        {
            QComboBox *defaultCombo = ui->scrollAreaWidgetContents->findChild<QComboBox*>("defaultValueComboBox");
            for(int i=0; i<defaultCombo->count(); i++)
            {
                if(defaultCombo->itemText(i).compare(tableWidget->model()->data(index).toString()) == 0)
                {
                    defaultCombo->removeItem(i);
                    break;
                }
            }
            tableWidget->model()->removeRow(row);
            break;
        }
    }
}

void AddAttributeWindow::populateWindow(const QString fieldType)
{
    QFrame *previousFrame = ui->scrollAreaWidgetContents->findChild<QFrame*>("frame");
    QTableWidget *previousTable = ui->scrollAreaWidgetContents->findChild<QTableWidget*>("tableWidget");
    if(previousFrame)
    {
        delete previousFrame;
    }
    if(previousTable)
    {
        delete previousTable;
    }
    QFrame *frame = new QFrame(ui->scrollAreaWidgetContents);
    frame->setObjectName(QStringLiteral("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLineWidth(1);
    QFormLayout *formLayout = new QFormLayout(frame);
    formLayout->setObjectName(QStringLiteral("formLayout"));
    frame->setLayout(formLayout);
    ui->gridLayout_2->addWidget(frame, 1, 0, 1, 2);
    ui->gridLayout_2->setRowStretch(0,1);
    ui->gridLayout_2->setRowStretch(1,2);
    ui->gridLayout_2->setRowStretch(2,2);

    QLabel *minimumValueLabel = new QLabel("Minimum Value",frame);
    minimumValueLabel->setObjectName(QStringLiteral("minimumValueLabel"));
    formLayout->setWidget(1, QFormLayout::LabelRole, minimumValueLabel);

    QSpinBox *minimumValueSpinBox = new QSpinBox(frame);
    minimumValueSpinBox->setMinimum(0);
    minimumValueSpinBox->setMaximum(1000000);
    connect(minimumValueSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setDefaultMin(int)));
    minimumValueSpinBox->setObjectName(QStringLiteral("minimumValueSpinBox"));
    minimumValueSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    formLayout->setWidget(1, QFormLayout::FieldRole, minimumValueSpinBox);

    QLabel *maximumValueLabel = new QLabel("Maximum Value",frame);
    maximumValueLabel->setObjectName(QStringLiteral("maximumValueLabel"));
    formLayout->setWidget(2, QFormLayout::LabelRole, maximumValueLabel);

    QSpinBox *maximumValueSpinBox = new QSpinBox(frame);
    maximumValueSpinBox->setMinimum(0);
    maximumValueSpinBox->setMaximum(1000000);
    maximumValueSpinBox->setValue(1000000);
    connect(maximumValueSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setDefaultMax(int)));
    maximumValueSpinBox->setObjectName(QStringLiteral("maximumValueSpinBox"));
    maximumValueSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    formLayout->setWidget(2, QFormLayout::FieldRole, maximumValueSpinBox);

    QLabel *attributeValueNameLabel = new QLabel("Attribute Value Name",frame);
    attributeValueNameLabel->setObjectName(QStringLiteral("attributeValueNameLabel"));
    formLayout->setWidget(3, QFormLayout::LabelRole, attributeValueNameLabel);

    QLineEdit *attributeValueNameLineEdit = new QLineEdit(frame);
    attributeValueNameLineEdit->setObjectName(QStringLiteral("attributeValueNameLineEdit"));
    formLayout->setWidget(3, QFormLayout::FieldRole, attributeValueNameLineEdit);

    QLabel *attributeValueLabel = new QLabel("Attribute Value",frame);
    attributeValueLabel->setObjectName(QStringLiteral("attributeValueLabel"));
    formLayout->setWidget(4, QFormLayout::LabelRole, attributeValueLabel);

    QLineEdit *attributeValueLineEdit = new QLineEdit(frame);
    attributeValueLineEdit->setObjectName(QStringLiteral("attributeValueLineEdit"));
    formLayout->setWidget(4, QFormLayout::FieldRole, attributeValueLineEdit);

    QPushButton *pushButton = new QPushButton(" Add Value",frame);
    QIcon addIcon(QIcon::fromTheme(QStringLiteral(":/icons/800px_COLOURBOX5697436.png")));
    pushButton->setIcon(addIcon);
    pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pushButton->setObjectName(QStringLiteral("addValue"));
    connect(pushButton,SIGNAL(clicked()),this,SLOT(addValue()));
    formLayout->setWidget(5, QFormLayout::FieldRole, pushButton);

    QTableWidget *tableWidget = new QTableWidget(frame);
    if (tableWidget->columnCount() < 3)
        tableWidget->setColumnCount(3);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    tableWidget->setObjectName(QStringLiteral("tableWidget"));
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    QList<QString> *headerLabels = new QList<QString>();
    headerLabels->push_back("Name");
    headerLabels->push_back("Value");
    headerLabels->push_back("");
    tableWidget->setHorizontalHeaderLabels(*headerLabels);
    tableWidget->setColumnWidth(0,150);
    tableWidget->setColumnWidth(1,150);
    tableWidget->setColumnWidth(2,40);
    tableWidget->verticalHeader()->setStretchLastSection(false);
    tableWidget->verticalHeader()->hide();
    ui->gridLayout_2->addWidget(tableWidget, 2, 0, 1, 1);

    ui->scrollArea->setWidget(ui->scrollAreaWidgetContents);
    ui->gridLayout->addWidget(ui->scrollArea, 0, 0, 1, 1);

    if(fieldType.compare(QString("Text")) == 0)
    {
        QLabel *defaultValueLabel = new QLabel("Default Value", frame);
        defaultValueLabel->setObjectName(QStringLiteral("defaultValueLabel"));
        formLayout->setWidget(0, QFormLayout::LabelRole, defaultValueLabel);

        QLineEdit *defaultValueLineEdit = new QLineEdit(frame);
        defaultValueLineEdit->setObjectName(QStringLiteral("defaultValueLineEdit"));
        formLayout->setWidget(0, QFormLayout::FieldRole, defaultValueLineEdit);

        minimumValueSpinBox->setEnabled(false);
        maximumValueSpinBox->setEnabled(false);
        attributeValueNameLineEdit->setEnabled(false);
        attributeValueLineEdit->setEnabled(false);
        pushButton->setEnabled(false);
        tableWidget->setEnabled(false);

        ui->canBeEmptyCheckBox->setEnabled(true);
    }
    else if(fieldType.compare(QString("Integer")) == 0)
    {
        QLabel *defaultValueLabel = new QLabel("Default Value", frame);
        defaultValueLabel->setObjectName(QStringLiteral("defaultValueLabel"));
        formLayout->setWidget(0, QFormLayout::LabelRole, defaultValueLabel);

        QSpinBox *defaultValueSpinBox = new QSpinBox(frame);
        defaultValueSpinBox->setMinimum(0);
        defaultValueSpinBox->setMaximum(1000000);
        defaultValueSpinBox->setObjectName(QStringLiteral("defaultValueSpinBox"));
        defaultValueSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        formLayout->setWidget(0, QFormLayout::FieldRole, defaultValueSpinBox);

        attributeValueNameLineEdit->setEnabled(false);
        attributeValueLineEdit->setEnabled(false);
        pushButton->setEnabled(false);
        tableWidget->setEnabled(false);

        ui->canBeEmptyCheckBox->setEnabled(false);
        ui->canBeEmptyCheckBox->setChecked(false);
    }
    else if(fieldType.compare(QString("Drop Down")) == 0)
    {
        QLabel *defaultValueLabel = new QLabel("Default Value", frame);
        defaultValueLabel->setObjectName(QStringLiteral("defaultValueLabel"));
        formLayout->setWidget(0, QFormLayout::LabelRole, defaultValueLabel);

        QComboBox *defaultValueComboBox = new QComboBox(frame);
        defaultValueComboBox->setObjectName(QStringLiteral("defaultValueComboBox"));
        formLayout->setWidget(0, QFormLayout::FieldRole, defaultValueComboBox);

        minimumValueSpinBox->setEnabled(false);
        maximumValueSpinBox->setEnabled(false);

        ui->canBeEmptyCheckBox->setEnabled(true);
    }
    else if(fieldType.compare(QString("Boolean")) == 0)
    {
        QLabel *defaultValueLabel = new QLabel("Default Value", frame);
        defaultValueLabel->setObjectName(QStringLiteral("defaultValueLabel"));
        formLayout->setWidget(0, QFormLayout::LabelRole, defaultValueLabel);

        QComboBox *defaultValueComboBox = new QComboBox(frame);
        defaultValueComboBox->addItem("False");
        defaultValueComboBox->addItem("True");
        defaultValueComboBox->setObjectName(QStringLiteral("defaultValueComboBox"));
        defaultValueComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        formLayout->setWidget(0, QFormLayout::FieldRole, defaultValueComboBox);

        minimumValueSpinBox->setEnabled(false);
        maximumValueSpinBox->setEnabled(false);
        attributeValueNameLineEdit->setEnabled(false);
        attributeValueLineEdit->setEnabled(false);
        pushButton->setEnabled(false);
        tableWidget->setEnabled(false);

        ui->canBeEmptyCheckBox->setEnabled(false);
        ui->canBeEmptyCheckBox->setChecked(false);
    }
}

void AddAttributeWindow::saveData()
{
    QTableWidget *tableWidget = ui->scrollAreaWidgetContents->findChild<QTableWidget*>("tableWidget");
    if((tableWidget->rowCount() > 0 && ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Drop Down")) == 0) ||
            (tableWidget->rowCount() == 0 && ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Drop Down")) != 0))
    {
        this->newField.setName(ui->attributeNameLineEdit->text());
        this->newField.setDisplayName(ui->attributeDisplayNameLineEdit->text());
        if(ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Text")) == 0)
        {
            this->newField.setFieldType(FieldType::Text);
            this->newField.setDefaultVal(ui->scrollAreaWidgetContents->findChild<QLineEdit*>("defaultValueLineEdit")->text());

        }
        else if(ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Integer")) == 0)
        {
            this->newField.setFieldType(FieldType::Integer);
            this->newField.setDefaultVal(ui->scrollAreaWidgetContents->findChild<QSpinBox*>("defaultValueSpinBox")->text());
            this->newField.setMinRange(ui->scrollAreaWidgetContents->findChild<QSpinBox*>("minimumValueSpinBox")->text().toInt());
            this->newField.setMaxRange(ui->scrollAreaWidgetContents->findChild<QSpinBox*>("maximumValueSpinBox")->text().toInt());
        }
        else if(ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Drop Down")) == 0)
        {
            this->newField.setFieldType(FieldType::DropDown);
            this->newField.setDefaultVal(ui->scrollAreaWidgetContents->findChild<QComboBox*>("defaultValueComboBox")->currentText());
            for (int row=0;row< tableWidget->rowCount();row++)
            {
                QString name =  tableWidget->item(row,0)->text();
                QString value =  tableWidget->item(row,1)->text();
                this->newField.dropDownValMap()->insert(name,value);
            }
        }
        else if(ui->valueTypeComboBox->itemText(ui->valueTypeComboBox->currentIndex()).compare(QString("Boolean")) == 0)
        {
            this->newField.setFieldType(FieldType::Bool);
            this->newField.setDefaultVal(ui->scrollAreaWidgetContents->findChild<QComboBox*>("defaultValueComboBox")->currentText());
        }
        this->newField.setIsMandetory(ui->mandatoryAttributeCheckBox->isChecked());
        this->newField.setNullable(ui->canBeEmptyCheckBox->isChecked());
        TableController *tableController = /*(TableController*)*/this->tableWidget->property("tableController").value<TableController*>();
        if(tableController)
        {
            QString returnString = tableController->addAttributeToModel(&(this->newField));
            if(returnString.compare(QString("submitted")) == 0)
            {
                qDebug() << "attribute added to model succesfully!";
                FieldInfo *validatedField = tableController->getConfig()->columnFieldList()->back();
                tableController->addAttributeToTableView(validatedField, fieldData);
                qDebug() << "attribute added to table widget successfully";
                accept();
            }
            else
            {
                qDebug() <<  "Failed to submit attribute data";
                QMessageBox msg(QMessageBox::Warning, "Failed to submit form", returnString);
                msg.exec();
            }
        }
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Failed to submit form", "Insert at least one name-value pair");
        msg.exec();
    }
}
