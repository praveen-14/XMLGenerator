#include "UpdateMetaDataWindow.h"
#include "ui_UpdateMetaDataWindow.h"

UpdateMetaDataWindow::UpdateMetaDataWindow(QWidget *parent, QList<QList<QString>> *metaCacheData, QList<QList<QString>> *metaMessageData, QList<QList<QString>> *defaultSortList, QTableWidget *tableWidget) :
    QDialog(parent),
    ui(new Ui::UpdateMetaDataWindow)
{
    ui->setupUi(this);
    this->metaCacheData = metaCacheData;
    this->metaMessageData = metaMessageData;
    this->defaultSortList = defaultSortList;
    this->sortOrderID = 0;
    this->tableWidget = tableWidget;
    this->signalMapper = new QSignalMapper(this);
    populateWindow();
}

UpdateMetaDataWindow::~UpdateMetaDataWindow()
{
    delete ui;
}

void UpdateMetaDataWindow::addSortOrder(){
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    QPushButton *removeBtn = new QPushButton("", ui->tableWidget);
    removeBtn->setFlat(true);
    removeBtn->setSizePolicy(QSizePolicy::Fixed , QSizePolicy::Fixed);
    QIcon *removeIcon = new QIcon(":/icons/Delete.png");
    removeBtn->setIcon(*removeIcon);
    removeBtn->setProperty("id",sortOrderID);
    for(int row = 0; row < tableWidget->rowCount(); row++){
        if(ui->fieldNameComboBox->currentText().compare(((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text()) == 0){
            removeBtn->setProperty("rowName", ((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text());
        }
    }
    signalMapper->setMapping(removeBtn,sortOrderID);
    QObject::connect(removeBtn,SIGNAL(clicked()),signalMapper,SLOT(map()));
    QObject::connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(removeValue(int)));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(ui->fieldNameComboBox->currentText()));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(ui->sortOrderComboBox->currentText()));
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1, 2, removeBtn);
    ui->fieldNameComboBox->removeItem(ui->fieldNameComboBox->currentIndex());
    if(ui->fieldNameComboBox->count() == 0){
        ui->addSortOrderButton->setEnabled(false);
    }
    sortOrderID++;
}

void UpdateMetaDataWindow::populateWindow(){
    ui->tableWidget->verticalHeader()->setSectionsMovable(true);
    ui->tableWidget->verticalHeader()->setDragEnabled(true);
    ui->tableWidget->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
    TableController *tableController = (TableController*)this->tableWidget->property("tableController").value<void*>();
    CacheConfig *config = tableController->getConfig();
    QList<FieldInfo*> *cacheFieldList = config->cacheFieldList();
    QList<FieldInfo*> *messageFieldList = config->messageFieldList();
    foreach(FieldInfo *item , *cacheFieldList){
        QLabel *label = new QLabel(item->displayName(), this);
        switch (item->fieldType())
        {
        case FieldType::Text:
        {
            QLineEdit *lineEdit= new QLineEdit(item->defaultVal(), ui->cacheFields);
            lineEdit->setObjectName(item->name());
            lineEdit->setText(getValue(item, metaCacheData));
            ui->cacheFieldsFormLayout->addRow(label,lineEdit);
        }

              break;
        case FieldType::DropDown:
        {
            QComboBox *combo = new QComboBox(ui->cacheFields);
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
            combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            if(item->dropDownValMap()->size() == 0){
                for(int row = 0; row < tableWidget->rowCount(); row++ ){
                    combo->addItem(((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text(), ((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text());
                }
            }
            QString value = getValue(item, metaCacheData);
            if(value.size() > 0){
                for(int comboItem = 0; comboItem < combo->count(); comboItem++){
                    if(combo->itemText(comboItem).compare(value) == 0){
                        combo->setCurrentIndex(comboItem);
                        break;
                    }
                }
            }else{

            }
            ui->cacheFieldsFormLayout->addRow(label,combo);

        }
            break;
        case FieldType::Bool:
        {
            QWidget *widget = new QWidget(this);
            QHBoxLayout *layout = new QHBoxLayout(this);
            layout->setContentsMargins(3,0,0,0);
            QCheckBox *checkBox = new QCheckBox(ui->cacheFields);
            checkBox->setChecked(item->defaultVal() == "true"? true : false);
            checkBox->setObjectName(item->name());
            if(getValue(item, metaCacheData).compare("true") == 0){
                checkBox->setChecked(true);
            }
            layout->addWidget(checkBox,1);
            layout->addWidget(label,10);
            widget->setLayout(layout);
            ui->cacheFieldsFormLayout->addRow("",widget);
        }
            break;
        case FieldType::Integer:
        {
            QSpinBox *spinBox = new QSpinBox(ui->cacheFields);
            spinBox->setMaximum(item->maxRange());
            spinBox->setMinimum(item->minRange());
            spinBox->setValue(item->defaultVal().toInt());
            spinBox->setObjectName(item->name());
            spinBox->setValue(getValue(item, metaCacheData).toInt());
            spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            ui->cacheFieldsFormLayout->addRow(label,spinBox);
        }
            break;
        case FieldType::Invalid:
        default:
            break;
        }
    }
    foreach(FieldInfo *item , *messageFieldList){
        QLabel *label = new QLabel(item->displayName(), ui->messageFields);
        switch (item->fieldType())
        {
        case FieldType::Text:
        {
            QLineEdit *lineEdit= new QLineEdit(item->defaultVal(), this);
            lineEdit->setObjectName(item->name());
            lineEdit->setText(getValue(item, metaMessageData));
            ui->messageFieldsFormLayout->addRow(label,lineEdit);
        }

              break;
        case FieldType::DropDown:
        {
            QComboBox *combo = new QComboBox(ui->messageFields);
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
            combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            if(item->dropDownValMap()->size() == 0){
                for(int row = 0; row < tableWidget->rowCount(); row++ ){
                    combo->addItem(((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text(), ((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text());
                }
            }
            QString value = getValue(item, metaMessageData);
//            combo->setCurrentText(value);
            for(int comboItem = 0; comboItem < combo->count(); comboItem++){
                if(combo->itemText(comboItem).compare(value) == 0){
                    combo->setCurrentIndex(comboItem);
                    break;
                }
            }
            ui->messageFieldsFormLayout->addRow(label,combo);
        }
            break;
        case FieldType::Bool:
        {
            QCheckBox *checkBox = new QCheckBox(ui->messageFields);
            checkBox->setChecked(item->defaultVal() == "true"? true : false);
            checkBox->setObjectName(item->name());
            if(getValue(item, metaMessageData).compare("true") == 0){
                checkBox->setChecked(true);
            }
            ui->messageFieldsFormLayout->addRow(label,checkBox);
        }
            break;
        case FieldType::Integer:
        {
            QSpinBox *spinBox = new QSpinBox(ui->messageFields);
            spinBox->setMaximum(item->maxRange());
            spinBox->setMinimum(item->minRange());
            spinBox->setValue(item->defaultVal().toInt());
            spinBox->setObjectName(item->name());
            spinBox->setValue(getValue(item, metaMessageData).toInt());
            spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            ui->messageFieldsFormLayout->addRow(label,spinBox);
        }
            break;
        case FieldType::Invalid:
        default:
            break;
        }
    }
    for(int sortItem = 0; sortItem < defaultSortList->size(); sortItem++) {
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
        for(int row = 0; row < this->tableWidget->rowCount(); row++){
            if(defaultSortList->at(sortItem).at(0).compare(((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text()) == 0){
                ui->tableWidget->setItem(sortItem,0,new QTableWidgetItem(((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text()));
                break;
            }
        }
        if(defaultSortList->at(sortItem).at(1).compare("1") == 0){
            ui->tableWidget->setItem(sortItem,1,new QTableWidgetItem("Ascending"));
        }else{
            ui->tableWidget->setItem(sortItem,1,new QTableWidgetItem("Descending"));
        }
        QPushButton *removeBtn = new QPushButton("", ui->tableWidget);
        removeBtn->setFlat(true);
        removeBtn->setSizePolicy(QSizePolicy::Fixed , QSizePolicy::Fixed);
        QIcon *removeIcon = new QIcon(":/icons/Delete.png");
        removeBtn->setIcon(*removeIcon);
        removeBtn->setProperty("id", sortOrderID);
        removeBtn->setProperty("rowName", defaultSortList->at(sortItem).at(0));
        signalMapper->setMapping(removeBtn,sortOrderID);
        ui->tableWidget->setCellWidget(sortItem,2,removeBtn);
        QObject::connect(removeBtn,SIGNAL(clicked()),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(removeValue(int)));
        sortOrderID++;
    }
    for(int row = 0; row < this->tableWidget->rowCount(); row++){
        bool found = false;
        for(int sortItem = 0; sortItem < defaultSortList->size(); sortItem++){
            if(defaultSortList->at(sortItem).at(0).compare(((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text()) == 0){
                found = true;
                break;
            }
        }
        if(!found){
            ui->fieldNameComboBox->addItem(((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text());
        }
    }
    ui->tableWidget->setColumnWidth(0,180);
    ui->tableWidget->setColumnWidth(1,180);
    ui->tableWidget->setColumnWidth(2,40);
}

QString UpdateMetaDataWindow::getValue(FieldInfo *field, QList<QList<QString>> *list){
    for(int item = 0; item < list->size(); item++){
        if(list->at(item).at(0).compare(field->name()) == 0){
            for(int row = 0; row < tableWidget->rowCount(); row++){
                if(((QLineEdit*)(this->tableWidget->cellWidget(row,0)))->text().compare(list->at(item).at(1)) == 0){
                    return ((QLineEdit*)(this->tableWidget->cellWidget(row,1)))->text();
                }
            }
            if(field->dropDownValMap()->size() > 0){
                QMap<QString,QString>::iterator valMapIterator = field->dropDownValMap()->begin();
                while(valMapIterator != field->dropDownValMap()->end()){
                    if(valMapIterator.value().compare(list->at(item).at(1)) == 0){
                        return valMapIterator.key();
                    }
                    valMapIterator++;
                }
            }else{
                return list->at(item).at(1);
            }
        }
    }
    return "";
}

void UpdateMetaDataWindow::removeValue(int id){
    QTableWidget *tableWidget = ui->tableWidget;
    for(int row=0; row < tableWidget->rowCount(); row++)
    {
        if(tableWidget->cellWidget(row,2)->property("id") == id)
        {
            QComboBox *fieldCombo = ui->fieldNameComboBox;
            fieldCombo->addItem(tableWidget->item(row,0)->text());
            tableWidget->model()->removeRow(row);
            break;
        }
    }
    ui->addSortOrderButton->setEnabled(true);
}

void UpdateMetaDataWindow::saveData(){
    QList<QList<QString>> updatedMetaCacheData;
    QList<QList<QString>> updatedMetaMessageData;
    QList<QList<QString>> updatedDefaultSortList;
    TableController *tableController = (TableController*)this->tableWidget->property("tableController").value<void*>();
    if(tableController){
        foreach(FieldInfo * item, *(tableController->getConfig()->cacheFieldList()))
        {
            switch (item->fieldType())
            {
                case FieldType::Text:
                {
                    QLineEdit *lineEdit = ui->cacheFields->findChild<QLineEdit*>(item->name());
                    if(lineEdit){
                        for(int cacheItem = 0; cacheItem < metaCacheData->size(); cacheItem++){
                            if(metaCacheData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                data.append(lineEdit->text());
                                updatedMetaCacheData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::DropDown:
                {
                    QComboBox *combo = ui->cacheFields->findChild<QComboBox*>(item->name());
                    if(combo){
                        for(int cacheItem = 0; cacheItem < metaCacheData->size(); cacheItem++){
                            if(metaCacheData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                if(!combo->itemData(combo->currentIndex()).isNull()){
                                    data.append(combo->itemData(combo->currentIndex()).toString());
                                }else{
                                    data.append(combo->currentText());
                                }
                                updatedMetaCacheData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::Bool:
                {
                    QCheckBox *checkBox = ui->cacheFields->findChild<QCheckBox*>(item->name());
                    if(checkBox){
                        for(int cacheItem = 0; cacheItem < metaCacheData->size(); cacheItem++){
                            if(metaCacheData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                if(checkBox->isChecked() == true){
                                    data.append("true");
                                }else
                                    data.append("false");
                                updatedMetaCacheData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::Integer:
                {
                    QSpinBox *spinBox = ui->cacheFields->findChild<QSpinBox*>(item->name());
                    if(spinBox){
                        for(int cacheItem = 0; cacheItem < metaCacheData->size(); cacheItem++){
                            if(metaCacheData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                data.append(spinBox->text());
                                updatedMetaCacheData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
        foreach(FieldInfo * item, *(tableController->getConfig()->messageFieldList()))
        {
            switch (item->fieldType())
            {
                case FieldType::Text:
                {
                    QLineEdit *lineEdit = ui->messageFields->findChild<QLineEdit*>(item->name());
                    if(lineEdit){
                        for(int cacheItem = 0; cacheItem < metaMessageData->size(); cacheItem++){
                            if(metaMessageData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                data.append(lineEdit->text());
                                updatedMetaMessageData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::DropDown:
                {
                    QComboBox *combo = ui->messageFields->findChild<QComboBox*>(item->name());
                    if(combo){
                        for(int cacheItem = 0; cacheItem < metaMessageData->size(); cacheItem++){
                            if(metaMessageData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                if(!combo->itemData(combo->currentIndex()).isNull()){
                                    data.append(combo->itemData(combo->currentIndex()).toString());
                                }else{
                                    data.append(combo->currentText());
                                }
                                updatedMetaMessageData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::Bool:
                {
                    QCheckBox *checkBox = ui->messageFields->findChild<QCheckBox*>(item->name());
                    if(checkBox){
                        for(int cacheItem = 0; cacheItem < metaMessageData->size(); cacheItem++){
                            if(metaMessageData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                if(checkBox->isChecked() == true){
                                    data.append("true");
                                }else
                                    data.append("false");
                                updatedMetaMessageData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
                case FieldType::Integer:
                {
                    QSpinBox *spinBox = ui->messageFields->findChild<QSpinBox*>(item->name());
                    if(spinBox){
                        for(int cacheItem = 0; cacheItem < metaMessageData->size(); cacheItem++){
                            if(metaMessageData->at(cacheItem).at(0).compare(item->name()) == 0){
                                QList<QString> data;
                                data.append(item->name());
                                data.append(spinBox->text());
                                updatedMetaMessageData.append(data);
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
        for(int row = 0; row < ui->tableWidget->rowCount(); row++){
            QList<QString> testList;
            updatedDefaultSortList.append(testList);
        }
        for(int row = 0; row < ui->tableWidget->rowCount(); row++){
            QList<QString> data;
            data.append(ui->tableWidget->cellWidget(row,2)->property("rowName").toString());
            if(ui->tableWidget->item(row,1)->text().compare("Ascending") == 0){
                data.append("1");
            }else{
                data.append("0");
            }
            updatedDefaultSortList.replace(ui->tableWidget->visualRow(row), data);
        }


        QList<QList<QList<QString>>> allMetaDataChanges;
        allMetaDataChanges.append(updatedMetaCacheData);
        allMetaDataChanges.append(updatedMetaMessageData);
        allMetaDataChanges.append(updatedDefaultSortList);
        QString returnValue = tableController->updateMetaData(&allMetaDataChanges);
        if(returnValue.compare("submitted") == 0){
            qDebug() << "Successfully updated meta data";
            accept();
        }else {
//            this->windowData.clear();
            QMessageBox msg(QMessageBox::Warning, "Failed to submit form", returnValue);
            msg.exec();
        }
    }
}
