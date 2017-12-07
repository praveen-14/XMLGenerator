#include "FindDiff.h"
#include "ui_FindDiff.h"

FindDiff::FindDiff(QWidget *parent, XMLFileData *mergedFile, QList<Change*> *newChanges, CacheConfig *config) :
    QDialog(parent),
    ui(new Ui::FindDiff)
{
    ui->setupUi(this);
    this->mergedFile = mergedFile;
    this->newChanges = newChanges;
    this->config = config;
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(tableItemClicked(int,int)));
//    connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    populateWindow();
}

FindDiff::~FindDiff()
{
    delete ui;
}

//void FindDiff::tableItemClicked(int row, int column)
//{
//    if (checkState != item->checkState())
//    {
//        qDebug() << item->checkState();
//    }
//}

void FindDiff::populateWindow()
{
    int rowCount = mergedFile->allFields.size();
    int columnCount = mergedFile->allAttributes.size();
    ui->tableWidget->setRowCount(rowCount+1);
    ui->tableWidget->setColumnCount(columnCount+1);

    QList<QString> columnNames;
    for(int column = 0; column < mergedFile->allAttributes.size(); column++){
        bool found = false;
        foreach (FieldInfo *field, *(config->columnFieldList())) {
            if(field->name().compare(mergedFile->allAttributes.at(column)) == 0){
                columnNames.push_back(field->displayName());
                found = true;
                break;
            }
        }
        if(!found){
            columnNames.push_back(mergedFile->allAttributes.at(column));
        }
    }

    QList<QString> tableViewColumns = columnNames;
    tableViewColumns.prepend("");
    QList<QString> tableViewRows = mergedFile->allFields;
    tableViewRows.prepend("");
    ui->tableWidget->setHorizontalHeaderLabels(tableViewColumns);
    ui->tableWidget->setVerticalHeaderLabels(tableViewRows);

    for(int row = 0; row < mergedFile->allFields.size(); row++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        item->setFlags((item->flags()/*|Qt::NoItemFlags*/) ^ Qt::ItemIsEditable);
        item->setBackgroundColor(QColor(0,0,255,40));
        ui->tableWidget->setItem(row+1, 0, item);
    }

    for(int column = 0; column < columnNames.size(); column++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        item->setFlags((item->flags()/*|Qt::NoItemFlags*/) ^ Qt::ItemIsEditable);
        item->setBackgroundColor(QColor(0,0,255,40));
        ui->tableWidget->setItem(0, column+1, item);
    }

//    ui->tableWidget->setVerticalHeaderLabels(mergedFile->allFields);
    QTableWidgetItem *item = new QTableWidgetItem("");
    item->setFlags((item->flags()/*|Qt::NoItemFlags*/) ^ Qt::ItemIsEditable);
    item->setBackgroundColor(QColor(0,0,255,40));
    ui->tableWidget->setItem(0, 0, item);

    for(int row = 0; row < rowCount; row++){
        for(int column = 0; column < columnCount; column++){
            QTableWidgetItem *item;
            if(mergedFile->tableData.at(row).at(column).compare("(Not Applicable)") == 0){
                item = new QTableWidgetItem("-");
            }else{
                item = new QTableWidgetItem(mergedFile->tableData.at(row).at(column));
            }

//            if(row == 0){
//                   item->setFlags(Qt::NoItemFlags);
//            }
            if(column%2 == 0){
                item->setBackgroundColor(QColor(0,0,0,20));
            }
            item->setFlags((item->flags()/*|Qt::NoItemFlags*/) ^ Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(row+1, column+1, item);
        }
    }
    foreach (Change *change, *newChanges) {
        if(change->getType() == ChangeType::DeleteAttribute){
            int column = mergedFile->allAttributes.indexOf(change->getAttributeName());
            ui->tableWidget->item(0,column+1)->setCheckState(Qt::Unchecked);
            ui->tableWidget->item(0,column+1)->setText("Delete this attribute?");
            changesMap.insert(change, ui->tableWidget->item(0,column+1));
            for(int row = 1; row < ui->tableWidget->rowCount(); row++){
                if(ui->tableWidget->item(row,column+1)->backgroundColor() == QColor(0,255,0,125)){
                    ui->tableWidget->item(row,column+1)->setBackgroundColor(QColor(255,255,0,125));
                }else{
                    ui->tableWidget->item(row,column+1)->setBackgroundColor(QColor(255,0,0,125));
                }
            }
        }
        if(change->getType() == ChangeType::DeleteField){
            for(int row = 0; row < mergedFile->allFields.size(); row++){
                if(mergedFile->tableData.at(row).at(0).compare(change->getFieldName()) == 0){
                    ui->tableWidget->item(row+1,0)->setCheckState(Qt::Unchecked);
                    ui->tableWidget->item(row+1,0)->setText("Delete this field?");
                    changesMap.insert(change, ui->tableWidget->item(row+1,0));
                    for(int column = 1; column < ui->tableWidget->columnCount(); column++){
                        if(ui->tableWidget->item(row+1,column)->backgroundColor() == QColor(0,255,0,125)){
                            ui->tableWidget->item(row+1,column)->setBackgroundColor(QColor(255,255,0,125));
                        }else{
                            ui->tableWidget->item(row+1,column)->setBackgroundColor(QColor(255,0,0,125));
                        }
                    }
                    break;
                }
            }
        }
        if(change->getType() == ChangeType::AddingField){
            for(int row = 0; row < mergedFile->allFields.size(); row++){
                if(mergedFile->tableData.at(row).at(0).compare(change->getFieldName()) == 0){
                    ui->tableWidget->item(row+1,0)->setCheckState(Qt::Unchecked);
                    ui->tableWidget->item(row+1,0)->setText("Add this field?");
                    changesMap.insert(change, ui->tableWidget->item(row+1,0));
                    for(int column = 1; column < ui->tableWidget->columnCount(); column++){
                        if(ui->tableWidget->item(row+1,column)->text().compare("-") == 0){
                            change->addUpdate(mergedFile->allAttributes.at(column-1), "(Not Applicable)");
                        }else{
                            change->addUpdate(mergedFile->allAttributes.at(column-1), ui->tableWidget->item(row+1,column)->text() );
                        }
                        if(ui->tableWidget->item(row+1,column)->backgroundColor() == QColor(255,0,0,125)){
                            ui->tableWidget->item(row+1,column)->setBackgroundColor(QColor(255,255,0,125));
                        }else{
                            ui->tableWidget->item(row+1,column)->setBackgroundColor(QColor(0,255,0,125));
                        }
                    }
                    break;
                }
            }
        }
        if(change->getType() == ChangeType::AddingAttribute){
            int column = mergedFile->allAttributes.indexOf(change->getAttributeName());
            int nameAttributeIndex = config->posForColumnName("name");
            ui->tableWidget->item(0,column+1)->setCheckState(Qt::Unchecked);
            ui->tableWidget->item(0,column+1)->setText("Add this Attribute?");
            changesMap.insert(change, ui->tableWidget->item(0,column+1));
            for(int row = 1; row < ui->tableWidget->rowCount(); row++){
                if(ui->tableWidget->item(row,column+1)->text().compare("-") == 0){
                    change->addUpdate(ui->tableWidget->item(row,nameAttributeIndex+1)->text(), "(Not Applicable)");
                }else{
                    change->addUpdate(ui->tableWidget->item(row,nameAttributeIndex+1)->text(), ui->tableWidget->item(row,column+1)->text());
                }
                if(ui->tableWidget->item(row,column+1)->backgroundColor() == QColor(255,0,0,125)){
                    ui->tableWidget->item(row,column+1)->setBackgroundColor(QColor(255,255,0,125));
                }else{
                    ui->tableWidget->item(row,column+1)->setBackgroundColor(QColor(0,255,0,125));
                }
            }
        }
        if(change->getType() == ChangeType::UpdateValue){
            int column = mergedFile->allAttributes.indexOf(change->getAttributeName());
            int nameAttributeIndex = config->posForColumnName("name");
            for(int row = 0; row < mergedFile->allFields.size(); row++){
                if(mergedFile->tableData.at(row).at(nameAttributeIndex).compare(change->getFieldName()) == 0){
                    ui->tableWidget->item(row+1,column+1)->setText(change->getOldValue() + " -> " + change->getValue());
                    ui->tableWidget->item(row+1,column+1)->setBackgroundColor(QColor(0,255,255,125));
                    ui->tableWidget->item(row+1,column+1)->setCheckState(Qt::Unchecked);
                    ui->tableWidget->item(row+1,column+1)->setToolTip("Accept change of value?");
                    changesMap.insert(change, ui->tableWidget->item(row+1,column+1));
                    break;
                }
            }
        }
    }

    foreach (Change *change, *newChanges){
        if(change->getType() == ChangeType::AddingEnum){
            for(int row = 0; row < mergedFile->allFields.size(); row++){
                for(int column = 0; column < mergedFile->allAttributes.size(); column++){
                    if(mergedFile->tableData.at(row).at(column).compare(change->getEnumName()) == 0){
                        ui->tableWidget->item(row+1,column+1)->setBackgroundColor(QColor(210,105,30,255));
                        ui->tableWidget->item(row+1,column+1)->setCheckState(Qt::Unchecked);
                        ui->tableWidget->item(row+1,column+1)->setToolTip("Add new enum?");
                        changesMap.insert(change, ui->tableWidget->item(row+1,column+1));
                    }
                }
            }
        }
    }

//    CheckBoxHeader *checkBox = new CheckBoxHeader(Qt::Horizontal, ui->tableWidget);
////    ui->tableWidget->horizontalHeader()->setI
//    ui->tableWidget->setHorizontalHeader(checkBox);
    ui->tableWidget->resizeColumnsToContents();
}

void FindDiff::saveMerge()
{
    QList<Change*> acceptedChanges;
    QMap<Change*,QTableWidgetItem*>::iterator changesIterator = changesMap.begin();
    while(changesIterator != changesMap.end()){
        if(changesIterator.value()->checkState() == Qt::Checked){
            acceptedChanges.push_back(changesIterator.key());
        }
        changesIterator++;
    }

    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = (TableController*)tableWidget->property("tableController").value<void*>();
    tableController->merge(&acceptedChanges);
    accept();
}
