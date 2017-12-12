#include <MainWindow.h>
#include <ui_MainWindow.h>
#include <TableController.h>
#include <AddFieldWindow.h>
#include <AddAttributeWindow.h>
#include <UpdateMetaDataWindow.h>

MainWindow* MainWindow::mainWindow = nullptr;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    MainWindow::mainWindow = this;
    this->warningIcon = *(new QIcon("C:/Users/praveense/Downloads/warning-512.png"));
    MainWindow::mainWindow->ui->setupUi(MainWindow::mainWindow);
    QGridLayout *grid = new QGridLayout(MainWindow::mainWindow->ui->frame);
    QTabWidget *tabWidget = new QTabWidget(MainWindow::mainWindow->ui->frame);
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged()));
    grid->addWidget(tabWidget);
    grid->setObjectName("gridLayout");
    grid->setContentsMargins(0,0,0,0);
    tabWidget->setObjectName("tabWidget");
    MainWindow::mainWindow->ui->frame->setLayout(grid);
    ui->progressBar->hide();
}

MainWindow::~MainWindow()
{
    qDebug() <<  "deleted";
    delete ui;
}

void MainWindow::loadFile(QString *filePath, int tabIndex)
{
    ui->progressBar->show();
    ui->progressBar->setValue(10);

    XMLFileData *XMLData;
    if(filePath)
    {
        XMLData = MainWindow::mainWindow->getFileController()->loadFile(filePath, true);
    }
    else
    {
        XMLData = MainWindow::mainWindow->getFileController()->loadFile(nullptr, true);
    }
    if(XMLData)
    {
        ui->progressBar->setValue(50);

        QString filePath = XMLData->filePath;
        if(MainWindow::mainWindow->tableControllerMap.contains(filePath))
        {
            QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
            tabWidget->setCurrentIndex(tabWidget->indexOf(MainWindow::mainWindow->getTableController(filePath)->getTableView()));
        }
        else
        {
            this->addTableController(XMLData, tabIndex);
        }
        ui->progressBar->setValue(90);
    }
    ui->progressBar->hide();
}

void MainWindow::addTableController(XMLFileData *xmlData, int tabIndex)
{
    QString fileName = xmlData->fileName;
    QString filePath = xmlData->filePath;
    QTabWidget *tabWidget = ui->frame->findChild<QTabWidget*>("tabWidget");
    TableController *tableController = new TableController(tabWidget, xmlData->config, this);
    tableController->setNewProject(xmlData->newProject);
    tableController->setCommitID(xmlData->commitID);
    MainWindow::mainWindow->tableControllerMap.insert(filePath,tableController);
    bool stoppedStatus = tableController->populateTableModel(xmlData);
    if(!stoppedStatus)
    {
        ui->addAttributeButton->setEnabled(true);
        ui->addFieldButton->setEnabled(true);
        ui->updateMetaDataButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->addEnumButton->setEnabled(true);
        ui->addProductButton->setEnabled(true);
        ui->commitButton->setEnabled(true);

        foreach(QList<QString> changeData, xmlData->changes)
        {
            ChangeType type = static_cast<ChangeType>(changeData.at(0).toInt());
            QString fieldName = changeData.at(1);
            QString attributeName = changeData.at(2);
            QString value = changeData.at(3);
            QString oldValue = changeData.at(4);
            Change *change = new Change(tableController, type, fieldName, attributeName, value);
            change->setOldValue(oldValue);
            tableController->getChanges()->push_back(change);
        }

        tableController->createTableView();
        if(xmlData->versions)
        {
            tableController->createTreeView();
        }
        ui->gridLayout->addWidget(tableController->getVersionTree(), 0, 0, 0, 0);

        tableController->addNewAttributes(xmlData);
        QTableWidget *tableWidget = tableController->getTableView();
        if(tabWidget && tableController)
        {
            if(tabIndex == -1)
            {
                tabWidget->insertTab(tabWidget->count(),tableWidget,QString(fileName));
            }
            else
            {
                tabWidget->insertTab(tabIndex,tableWidget,QString(fileName));
            }
            tabWidget->setCurrentIndex(tabWidget->indexOf(tableWidget));
            QPushButton *closeButton  = new QPushButton(tableWidget);
            closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            closeButton->setFixedSize(16,16);
            closeButton->setFlat(true);
            closeButton->setIcon(*(new QIcon(":/icons/Close1.png")) );
            signalMapper.setMapping(closeButton, tableWidget);
            QObject::connect(closeButton,SIGNAL(pressed()),&signalMapper,SLOT(map()));
            QObject::connect(&signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(closeTab(QWidget*)));
            tabWidget->tabBar()->setTabButton(tabWidget->indexOf(tableWidget), QTabBar::RightSide, closeButton);
        }
    }
    else
    {
        CacheConfig *config = tableController->getConfig();
        FieldInfo *enumField = config->columnFieldList()->at(config->posForColumnName("enumName"));
        QList<QString> enumKeys = enumField->dropDownValMap()->keys();
        for(int newEnum = 0; newEnum < tableController->getNewEnums()->size(); newEnum++)
        {
            foreach(QString key, enumKeys)
            {
                if(key.compare(tableController->getNewEnums()->at(newEnum)->getName()) == 0)
                {
                    enumField->dropDownValMap()->remove(key);
                }
            }
        }
        tableControllerMap.remove(xmlData->filePath);
    }
}

void MainWindow::saveEnum(Enum *newEnum, QString *filePath)
{
    TableController *tableController = tableControllerMap.find(*filePath).value();
    tableController->saveEnum(newEnum);
}

void MainWindow::addEnum()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    QList<QString> filePaths = tableControllerMap.keys();
    foreach (QString filePath, filePaths)
    {
        if(tableControllerMap.find(filePath).value() == tableController)
        {
            AddEnum *addEnumWindow = new AddEnum(this, &filePath);
            addEnumWindow->setModal(true);
            int result = addEnumWindow->exec();
            if(result == 1)
            {
                tableController->createTableView();
            }
            break;
        }
    }
}

void MainWindow::versionChanged(QTreeWidgetItem* sender, int column)
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();

    QMessageBox confirmation(QMessageBox::Question, "Switch Version", "Are you sure? \nAll uncomitted changes of the current version will be lost.");
    confirmation.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    confirmation.setDefaultButton(QMessageBox::Yes);
    int result = confirmation.exec();

    if (result == QMessageBox::Yes)
    {
        ui->progressBar->show();
        ui->progressBar->setValue(20);

        QVariant variant = sender->data(0,Qt::UserRole);
        QString newVersionPath = variant.value<QString>();
        delete tableController->getVersionTree();
        QString filePath = tableControllerMap.key(tableController);
        int index = tabWidget->currentIndex();
        tabWidget->removeTab(index);

        ui->progressBar->setValue(40);

        tableControllerMap.remove(filePath);
        fileController.changeVersion(filePath, newVersionPath);

        ui->progressBar->setValue(50);

        loadFile(&filePath, index);

        ui->progressBar->setValue(60);

        QTableWidget *newTableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
        TableController *newTableController = /*(TableController*)*/newTableWidget->property("tableController").value<TableController*>();
        QTreeWidget *tree = newTableController->getVersionTree();
        Version *version = newTableController->getVersion()->findVersion(newVersionPath);
        QTreeWidgetItemIterator treeItr(tree);

        ui->progressBar->setValue(70);

        while (*treeItr)
        {
            if ((*treeItr)->text(0) == version->getVersionID())
            {
                (*treeItr)->setIcon(1,QIcon(":/icons/arrow-png-22.png"));
            }
            else{
                (*treeItr)->setIcon(1,QIcon());
            }
            treeItr++;
        }
        tree->resizeColumnToContents(0);
        tree->resizeColumnToContents(1);
    }
    else
    {

    }

    ui->progressBar->setValue(100);
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
}

void MainWindow::addProduct()
{
    XMLFileData *productXMLData = MainWindow::mainWindow->getFileController()->loadFile(nullptr, false);
    if(productXMLData)
    {
        QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
        QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
        TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
        if(tableController)
        {
            tableController->setProductFile(productXMLData);
            tableController->findDiff();
        }
    }
    save(1);
}

void MainWindow::closeTab(QWidget *sender)
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    int index = tabWidget->indexOf(sender);
    TableController *tableController = /*(TableController*)*/sender->property("tableController").value<TableController*>();
    if(tableController && tabWidget->tabText(index).size() > 0)
    {
        QMessageBox::StandardButton confirmation;
        confirmation = QMessageBox::question(this, "Confirm", "Close file '" + tabWidget->tabText(index) + "'? All unsaved data will be lost.", QMessageBox::Yes|QMessageBox::No);
        if (confirmation == QMessageBox::Yes)
        {
            qDebug() << "Yes was clicked";

            tabWidget->removeTab(index);
            QList<QTreeWidget*> treeWidgets = ui->scrollAreaWidgetContents->findChildren<QTreeWidget*>();
            QList<QTreeWidget*>::iterator treeIterator = treeWidgets.begin();
            while(treeIterator != treeWidgets.end())
            {
                TableController *tableControllerItr = /*(TableController*)*/(*treeIterator)->property("tableController").value<TableController*>();
                if(tableControllerItr == tableController)
                {
                    (*treeIterator)->hide();
                    (*treeIterator)->setEnabled(false);
                }
                treeIterator++;
            }


            QString removedFilePath;
            QMap<QString,TableController*>::iterator tableControllerIterator = this->getTableControllerMap()->begin();
            while(tableControllerIterator != this->getTableControllerMap()->end())
            {
                if(tableControllerIterator.value() == tableController)
                {
                    removedFilePath = tableControllerIterator.key();
                    break;
                }
                tableControllerIterator++;
            }
            this->getTableControllerMap()->remove(removedFilePath);
        }
        else
        {
            qDebug() << "Yes was *not* clicked";
        }
    }
}

void MainWindow::updateMetaData()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    UpdateMetaDataWindow *updateMetaDataWindow = new UpdateMetaDataWindow(this,tableController->getMetaCacheData(),tableController->getMetaMessageData(), tableController->getDefaultSortList(), tableWidget);
    updateMetaDataWindow->setModal(true);
    updateMetaDataWindow->exec();
}

void MainWindow::save(int commit)
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    XMLFileData xmlData;
    xmlData.fileName = tabWidget->tabText(tabWidget->currentIndex());
    xmlData.filePath = tableControllerMap.key(tableController);
    QList<FieldInfo*> fields;
    CacheConfig *config = tableController->getConfig();
    for(int row = 0; row < tableWidget->rowCount(); row++)
    {
        QList<QString> testList;
        xmlData.tableData.push_back(testList);
    }
    for(int row = 0; row < tableWidget->rowCount(); row++)
    {
        xmlData.allFields.push_back(tableWidget->verticalHeaderItem(row)->text());
        QList<QString> fieldData;
        for(int column = 0; column < tableWidget->columnCount(); column++)
        {
            if(row == 0)
            {
                for(int i = 0; i < config->columnFieldList()->size(); i++)
                {
                    if(config->columnFieldList()->at(i)->displayName().compare(tableWidget->horizontalHeaderItem(column)->text()) == 0)
                    {
                        fields.push_back(config->columnFieldList()->at(i));
                        break;
                    }
                }
                xmlData.allAttributes.push_back(fields.back()->name());
            }
            if(fields.at(column)->fieldType() == FieldType::Text)
            {
                fieldData.append(((QLineEdit*)(tableWidget->cellWidget(row,column)))->text());
            }
            if(fields.at(column)->fieldType() == FieldType::DropDown)
            {
                QString value = ((QComboBox*)(tableWidget->cellWidget(row,column)))->currentText();
                if(value.compare(QString("(Not Applicable)")) == 0)
                {
                    fieldData.append(value);
                }
                else
                {
                    QString resolvedValue = fields.at(column)->dropDownValMap()->find(value).value();
                    fieldData.append(resolvedValue);
                }

            }
            if(fields.at(column)->fieldType() == FieldType::Bool)
            {
                if(tableWidget->cellWidget(row,column)->findChild<QCheckBox*>()->checkState() == Qt::Checked)
                {
                    fieldData.append("true");
                }
                else if(tableWidget->cellWidget(row,column)->findChild<QCheckBox*>()->checkState() == Qt::PartiallyChecked)
                {
                    fieldData.append("(Not Applicable)");
                }
                else
                {
                    fieldData.append("false");
                }
            }
            if(fields.at(column)->fieldType() == FieldType::Integer)
            {
                int value = ((QSpinBox*)(tableWidget->cellWidget(row,column)))->value();
                if(((QSpinBox*)(tableWidget->cellWidget(row,column)))->specialValueText().compare("") != 0 && ((QSpinBox*)(tableWidget->cellWidget(row,column)))->minimum() == value)
                {
                    fieldData.append("(Not Applicable)");
                }
                else
                {
                    fieldData.append(QString::number(value));
                }

            }
        }
        xmlData.tableData.replace(tableWidget->visualRow(row),fieldData);
    }
    xmlData.metaCacheData = *(tableController->getMetaCacheData());
    xmlData.metaMessageData = *(tableController->getMetaMessageData());
    xmlData.defaultSortList = *(tableController->getDefaultSortList());
    xmlData.commitID = tableController->getCommitID();

    int commitNumber = tableController->getVersion()->size(tableController->getVersion()) + 1;

    QList<QString> returnedValue;
    if(tableController->isNewProject())
    {
        CommitWindow *commitMessage = new CommitWindow();
        commitMessage->setModal(true);
        if(tableController->isNewProduct())
        {
             commitMessage->mergeCommit();
        }
        int result = commitMessage->exec();
        if(result == 1)
        {
            returnedValue = fileController.save(xmlData, *(tableController->getUnresolvedProductFile()), tableController->getChanges(), &fields, tableController->getNewEnums(), tableController->isNewProject(), tableController->isNewProduct(), commitNumber, 1, &(this->commitMessage));
        }
    }
    else
    {
        if(commit == 1){
            CommitWindow *commitMessage = new CommitWindow();
            commitMessage->setModal(true);
            if(tableController->isNewProduct())
            {
                 commitMessage->mergeCommit();
            }
            int result = commitMessage->exec();
            if(result == 1)
            {
                returnedValue = fileController.save(xmlData, *(tableController->getUnresolvedProductFile()), tableController->getChanges(), &fields, tableController->getNewEnums(), tableController->isNewProject(), tableController->isNewProduct(), commitNumber, commit, &(this->commitMessage));
            }
        }
        else
        {
            qDebug() << "Saving to existing directory without commit";
            returnedValue = fileController.save(xmlData, *(tableController->getUnresolvedProductFile()), tableController->getChanges(), &fields, tableController->getNewEnums(), tableController->isNewProject(), tableController->isNewProduct(), commitNumber, commit);
        }
    }
    if(returnedValue.size() > 0)
    {
        if(returnedValue.at(1).size() > 0)
        {
            Version *newVersion = new Version(returnedValue.at(1));
            newVersion->setCommitMessage(this->commitMessage);
            if(tableController->isNewProject())
            {
                tableController->setVersion(newVersion);
                tableController->createTreeView();
            }
            else
            {
                Version *previousVersion = tableController->getVersion()->findVersion(tableController->getCommitID());
                previousVersion->getSubVersions()->push_back(newVersion);
                tableController->createTreeView();
            }
        }
        tableController->getNewEnums()->clear();
        tableController->setNewProject(false);
        tableController->setNewProduct(false);
        tableControllerMap.remove(xmlData.filePath);
        tableControllerMap.insert(returnedValue.at(0), tableController);
        if(returnedValue.at(1).size() > 0)
        {
            tableController->setCommitID(returnedValue.at(1));
        }

        QTreeWidget *tree = tableController->getVersionTree();
        Version *version = tableController->getVersion()->findVersion(tableController->getCommitID());
        QTreeWidgetItemIterator treeItr(tree);
        while (*treeItr)
        {
            if ((*treeItr)->text(0) == version->getVersionID())
            {
                (*treeItr)->setIcon(1,QIcon(":/icons/arrow-png-22.png"));
            }
            else
            {
                (*treeItr)->setIcon(1,QIcon());
            }
            treeItr++;
        }
    }
}

void MainWindow::saveAndCommit()
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Commiting Changes!", "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        save(1);
    }
    else
    {
    }
}

void MainWindow::tabChanged()
{
    ui->deleteColumnButton->setEnabled(false);
    ui->deleteRowButton->setEnabled(false);
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    if(tableWidget)
    {
        ui->addAttributeButton->setEnabled(true);
        ui->addFieldButton->setEnabled(true);
        ui->updateMetaDataButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->addEnumButton->setEnabled(true);
        ui->addProductButton->setEnabled(true);
        ui->commitButton->setEnabled(true);

        TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
        if(tableController->isNewProject())
        {
            ui->saveButton->setToolTip("Create New Project and Save");
            QIcon saveIcon = QIcon(":/icons/if_Save-as_70651.png");
            ui->saveButton->setIcon(saveIcon);
        }
        else
        {
            ui->saveButton->setToolTip("Save to Existing project");
            QIcon saveIcon = QIcon(":/icons/Save-icon.png");
            ui->saveButton->setIcon(saveIcon);
        }        
        QList<QTreeWidget*> treeWidgets = ui->scrollAreaWidgetContents->findChildren<QTreeWidget*>();
        QList<QTreeWidget*>::iterator treeIterator = treeWidgets.begin();
        while(treeIterator != treeWidgets.end())
        {
            TableController *tableControllerItr = /*(TableController*)*/(*treeIterator)->property("tableController").value<TableController*>();
            if(tableControllerItr == tableController)
            {
                (*treeIterator)->show();
                (*treeIterator)->setEnabled(true);
            }
            else
            {
                (*treeIterator)->hide();
                (*treeIterator)->setEnabled(false);
            }
            treeIterator++;
        }
    }
    else
    {
        ui->addAttributeButton->setEnabled(false);
        ui->addFieldButton->setEnabled(false);
        ui->updateMetaDataButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->addEnumButton->setEnabled(false);
        ui->addProductButton->setEnabled(false);
        ui->commitButton->setEnabled(false);
    }
}

void MainWindow::setSelectedRow(int row)
{
    ui->deleteRowButton->setEnabled(true);
    ui->deleteColumnButton->setEnabled(false);
    selectedRow = row;
}

void MainWindow::setSelectedColumn(int column)
{
    ui->deleteColumnButton->setEnabled(true);
    ui->deleteRowButton->setEnabled(false);
    selectedColumn = column;
}

void MainWindow::deleteRow(){
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm", "Remove '" + tableWidget->verticalHeaderItem(selectedRow)->text() + "' from table?", QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        qDebug() << "Yes was clicked";
        tableController->deleteRow(selectedRow);
        ui->deleteRowButton->setEnabled(false);
    }
    else
    {
        qDebug() << "Yes was *not* clicked";
    }
}

void MainWindow::deleteColumn()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this, "Confirm", "Remove '" + tableWidget->horizontalHeaderItem(selectedColumn)->text() + "' from table?" , QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        qDebug() << "Yes was clicked";
        tableController->deleteColumn(selectedColumn);
        ui->deleteColumnButton->setEnabled(false);
    }
    else
    {
        qDebug() << "Yes was *not* clicked";
    }
}

QIcon* MainWindow::getWarningIcon()
{
    return &(this->warningIcon);
}

FileController* MainWindow::getFileController()
{
    return &(this->fileController);
}

QMap<QString, TableController*>* MainWindow::getTableControllerMap()
{
    return &tableControllerMap;
}


TableController* MainWindow::getTableController(QString key)
{
    QMap<QString, TableController*>::iterator iterator = this->getTableControllerMap()->find(key);
    return iterator.value();
}

void MainWindow::addField()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    AddFieldWindow *addFieldWindow = new AddFieldWindow(this,tableWidget);
    addFieldWindow->setModal(true);
    addFieldWindow->exec();
}

void MainWindow::addAttribute()
{
    QTabWidget *tabWidget= MainWindow::mainWindow->findChild<QTabWidget*>("tabWidget");
    QTableWidget *tableWidget = (QTableWidget*)tabWidget->widget(tabWidget->currentIndex());
    TableController *tableController = /*(TableController*)*/tableWidget->property("tableController").value<TableController*>();
    AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(this,tableWidget);
    addAttributeWindow->setModal(true);
    addAttributeWindow->exec();
}

void MainWindow::setCommitMessage(QString message)
{
    this->commitMessage = message;
}

Table* MainWindow::getTableModel(QTableWidget *tableWidget)
{
    return this->getTableControllerMap()->find(tableWidget->property("filePath").toString()).value()->getDataTable();
}
