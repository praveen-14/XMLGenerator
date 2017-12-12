#include <TableController.h>

TableController::TableController(QWidget *tableWidgetParent, CacheConfig *config, QObject *parent): QObject(parent)
{
    this->tableView = new QTableWidget(tableWidgetParent);
    this->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    this->versionTree = new QTreeWidget(MainWindow::mainWindow);
    this->versionTree->setExpandsOnDoubleClick(false);
    this->versionTree->setProperty("tableController",qVariantFromValue((TableController*)this));
    this->versionTree->setColumnCount(2);
    QStringList headers;
    headers << "Versions" << "Commit Message";
    this->versionTree->setHeaderLabels(headers);
    this->versionTree->resizeColumnToContents(0);
    this->versionTree->resizeColumnToContents(1);

    this->config = config;
    newProduct = false;
}

TableController::~TableController()
{
    qDebug() << "table controller deleted";
}

bool TableController::isNewProject()
{
    return newProject;
}

bool TableController::isNewProduct()
{
    return newProduct;
}

QString TableController::getCommitID()
{
    return commitID;
}

QTreeWidget* TableController::getVersionTree()
{
    return versionTree;
}

Version* TableController::getVersion()
{
    return version;
}

CacheConfig* TableController::getConfig()
{
    return config;
}

Table* TableController::getDataTable()
{
    return &dataTable;
}

QTableWidget* TableController::getTableView()
{
    return tableView;
}

QList<QList<QString>>* TableController::getMetaCacheData()
{
    return &(this->metaCacheData);
}

QList<QList<QString>>* TableController::getMetaMessageData()
{
    return &(this->metaMessageData);
}

QList<QList<QString>>* TableController::getDefaultSortList()
{
    return &(this->defaultSortList);
}

QList<Change*>* TableController::getChanges()
{
    return &(changes);
}

XMLFileData* TableController::getProductFile()
{
    return &(this->productFile);
}

XMLFileData* TableController::getUnresolvedProductFile()
{
    return &unresolvedProductFile;
}

void TableController::setMetaCacheData(QList<QList<QString>>* metaCacheData)
{
    this->metaCacheData = *metaCacheData;
}

void TableController::setMetaMessageData(QList<QList<QString>>* metaMessageData)
{
    this->metaMessageData = *metaMessageData;
}

void TableController::setDefaultSortList(QList<QList<QString>>* defaultSortList)
{
    this->defaultSortList = *defaultSortList;
}

void TableController::setProductFile(XMLFileData *fileData)
{
    this->productFile = *fileData;
    this->unresolvedProductFile = *fileData;
}

void TableController::setNewProject(bool newProject)
{
    this->newProject = newProject;
}

void TableController::setNewProduct(bool newProduct)
{
    this->newProduct = newProduct;
}

void TableController::setCommitID(QString commitID)
{
    this->commitID = commitID;
}

void TableController::setVersion(Version *version)
{
    this->version = version;
}

void TableController::saveEnum(Enum *enumObject)
{
    FieldInfo *enumField = config->columnFieldList()->at(config->posForColumnName("enumName"));
    QMap<QString,QString> *enumValues = enumField->dropDownValMap();
    enumValues->insert(enumObject->getName(),enumObject->getName());
    newEnums.push_back(enumObject);
}

QList<Enum*>* TableController::getNewEnums()
{
    return &newEnums;
}

void TableController::findDiff()
{

    QList<Change*> newChanges;

    QList<QString> solutionRowHeaders;
    QList<QString> productRowHeaders;
    QList<QString> solutionColumnHeaders;
    QList<QString> productColumnHeaders = productFile.allAttributes;
    foreach(QString columnHeader, *(dataTable.getAllAttributes()))
    {
        foreach (FieldInfo *field, *(config->columnFieldList()))
        {
            if(field->displayName().compare(columnHeader) == 0)
            {
                solutionColumnHeaders.push_back(field->name());
                break;
            }
        }
    }
    for(int row = 0; row < productFile.allFields.size(); row++)
    {
        productRowHeaders.push_back(productFile.tableData.at(row).at(0));
    }
    for(int solutionRow = 0; solutionRow < dataTable.getAllFields()->size(); solutionRow++)
    {
        solutionRowHeaders.push_back(dataTable.getRows()->at(solutionRow)->getRowCells()->at(0)->getValue());
    }
    QList<QString> newSolutionRowHeaders = solutionRowHeaders;
    QList<QString> newProductRowHeaders = productRowHeaders;
    QList<QString> newSolutionColumnHeaders = solutionColumnHeaders;
    QList<QString> newProductColumnHeaders = productColumnHeaders;
    QList<QList<int>> rowMapping; //[solutionIndex,productIndex]
    QList<QList<int>> columnMapping; //[solutionIndex,productIndex]

    //finding solution and product row mapping
    for(int solutionRow = 0; solutionRow < solutionRowHeaders.size(); solutionRow++)
    {
        for(int productRow = 0; productRow < productRowHeaders.size(); productRow++)
        {
            if(solutionRowHeaders.at(solutionRow).compare(productRowHeaders.at(productRow)) == 0)
            {
                QList<int> rowMap;
                rowMap.push_back(solutionRow);
                rowMap.push_back(productRow);
                newSolutionRowHeaders.replace(solutionRow,QString("COMMON"));
                newProductRowHeaders.replace(productRow,QString("COMMON"));
                rowMapping.push_back(rowMap);
            }
        }
    }

    //finding solution and product column mapping
    for(int solutionColumn = 0; solutionColumn < solutionColumnHeaders.size(); solutionColumn++)
    {
       for(int productColumn = 0; productColumn < productColumnHeaders.size(); productColumn++)
       {
            if(solutionColumnHeaders.at(solutionColumn).compare(productColumnHeaders.at(productColumn)) == 0)
            {
                if(productFile.availableAttributes.contains(solutionColumnHeaders.at(solutionColumn)))
                {
                    QList<int> columnMap;
                    columnMap.push_back(solutionColumn);
                    columnMap.push_back(productColumn);
                    newSolutionColumnHeaders.replace(solutionColumn,QString("COMMON"));
                    newProductColumnHeaders.replace(productColumn,QString("COMMON"));
                    columnMapping.push_back(columnMap);
                }
            }
        }
    }

    //find new Enums
    for(int productColumn = 0; productColumn < productColumnHeaders.size(); productColumn++)
    {
        if(productColumnHeaders.at(productColumn).compare("enumName") == 0)
        {
            for(int productRow = 0; productRow < productRowHeaders.size(); productRow++)
            {
                QString value = productFile.tableData.at(productRow).at(productColumn);
                if(value.compare("(Not Applicable)") != 0)
                {
                    FieldInfo *field = config->columnFieldList()->at(config->posForColumnName("enumName"));
                    QList<QString> enums = field->dropDownValMap()->keys();
                    if(!enums.contains(value))
                    {
                        bool found = false;
                        foreach (Change *changeItr, newChanges)
                        {
                            if(changeItr->getEnumName().compare(value) == 0)
                            {
                                found = true;
                            }
                        }
                        if(!found)
                        {
                            Change *change = new Change(this, ChangeType::AddingEnum);
                            change->setEnumName(value);
                            newChanges.push_back(change);
                        }
                    }
                }
            }
            break;
        }
    }

    //finding common fields and attributes value updates
    for(int rowMapIndex = 0; rowMapIndex < rowMapping.size(); rowMapIndex++)
    {
        for(int columnMapIndex = 0; columnMapIndex < columnMapping.size(); columnMapIndex++)
        {
            int solutionRowIndex = rowMapping.at(rowMapIndex).at(0);
            int productRowIndex = rowMapping.at(rowMapIndex).at(1);
            int solutionColumnIndex = columnMapping.at(columnMapIndex).at(0);
            int productColumnIndex = columnMapping.at(columnMapIndex).at(1);
            QString productValue = productFile.tableData.at(productRowIndex).at(productColumnIndex);
            QString solutionValue = dataTable.getRows()->at(solutionRowIndex)->getRowCells()->at(solutionColumnIndex)->getValue();
            FieldInfo *field = config->columnFieldList()->at(config->posForColumnName(solutionColumnHeaders.at(solutionColumnIndex)));
            if(field->fieldType() == FieldType::DropDown)
            {
                QMap<QString,QString>::iterator valMapIterator = field->dropDownValMap()->begin();
                while(valMapIterator != field->dropDownValMap()->end())
                {
                    if(valMapIterator.value().compare(productValue) == 0)
                    {
                        productValue = valMapIterator.key();
                        QList<QString> tempList = productFile.tableData.at(productRowIndex);
                        tempList.replace(productColumnIndex,productValue);
                        productFile.tableData.replace(productRowIndex, tempList);
                        break;
                    }
                    valMapIterator++;
                }
            }
            if(productValue.compare(solutionValue) != 0 && dataTable.getAvailableAttributes()->contains(solutionColumnHeaders.at(solutionColumnIndex)))
            {
                bool found = false;
                QString fieldName = solutionRowHeaders.at(solutionRowIndex);
                QString attributeName = solutionColumnHeaders.at(solutionColumnIndex);
                for(int changeItem = 0; changeItem < changes.size(); changeItem++)
                {
                    Change *change = changes.at(changeItem);
                    if(change->getType() == ChangeType::UpdateValue && change->getFieldName().compare(fieldName) == 0 &&
                            change->getAttributeName().compare(attributeName) == 0 &&
                            change->getOldValue().compare(productValue) == 0)
                    {
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    Change *change = new Change(this, ChangeType::UpdateValue, fieldName, attributeName, productValue);
                    change->setOldValue(solutionValue);
                    change->setSolutionColumn(solutionColumnIndex);
                    change->setSolutionRow(solutionRowIndex);
                    newChanges.push_back(change);
                }
            }
        }
    }

    //finding new row headers
    for(int productRow = 0; productRow < newProductRowHeaders.size(); productRow++)
    {
        if(newProductRowHeaders.at(productRow).compare("COMMON") != 0)
        {
            bool found = false;
            QString fieldName = newProductRowHeaders.at(productRow);
            for(int columnMapIndex = 0; columnMapIndex < columnMapping.size(); columnMapIndex++)
            {
                int productColumnIndex = columnMapping.at(columnMapIndex).at(1);
                QString productValue = productFile.tableData.at(productRow).at(productColumnIndex);
                FieldInfo *field = config->columnFieldList()->at(config->posForColumnName(productColumnHeaders.at(productColumnIndex)));
                if(field->fieldType() == FieldType::DropDown)
                {
                    QMap<QString,QString>::iterator valMapIterator = field->dropDownValMap()->begin();
                    while(valMapIterator != field->dropDownValMap()->end())
                    {
                        if(valMapIterator.value().compare(productValue) == 0)
                        {
                            productValue = valMapIterator.key();
                            QList<QString> tempList = productFile.tableData.at(productRow);
                            tempList.replace(productColumnIndex,productValue);
                            productFile.tableData.replace(productRow, tempList);
                            break;
                        }
                        valMapIterator++;
                    }
                }
            }
            for(int changeItem = 0; changeItem < changes.size(); changeItem++)
            {
                Change *change = changes.at(changeItem);
                if(change->getType() == ChangeType::AddingField && change->getFieldName().compare(fieldName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                Change *change = new Change(this, ChangeType::AddingField, fieldName);
                newChanges.push_back(change);
            }
        }
    }
    //finding rows deleted by the user
//    foreach(QString fieldName, productRowHeaders){
//        if(!solutionRowHeaders.contains(fieldName)){
//            bool found = false;
//            for(int changeItem = 0; changeItem < changes.size(); changeItem++){
//                Change *change = changes.at(changeItem);
//                if(change->getType() == ChangeType::AddingField && change->getFieldName().compare(fieldName) == 0)
//                {
//                    found = true;
//                    break;
//                }
//            }
//            if(!found){
//                Change *change = new Change(this, ChangeType::AddingField, fieldName);
//                newChanges.push_back(change);
//            }
//        }
//    }

    //finding new attributes present in product xml
    for(int newColumn = 0; newColumn < productFile.newAttributes.size(); newColumn++)
    {
        bool found = false;
        QString attributeName = productFile.newAttributes.at(newColumn);
        if(!solutionColumnHeaders.contains(attributeName))
        {
            for(int changeItem = 0; changeItem < changes.size(); changeItem++)
            {
                Change *change = changes.at(changeItem);
                if(change->getType() == ChangeType::AddingAttribute && change->getAttributeName().compare(attributeName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                Change *change = new Change(this, ChangeType::AddingAttribute, "", attributeName);
                newChanges.push_back(change);
            }
        }
    }

    //finding attributes deleted by the user
//    foreach(QString attributeName, productColumnHeaders){
//        if(!solutionColumnHeaders.contains(attributeName)){
//            bool found = false;
//            for(int changeItem = 0; changeItem < changes.size(); changeItem++){
//                Change *change = changes.at(changeItem);
//                if(change->getType() == ChangeType::AddingAttribute && change->getAttributeName().compare(attributeName) == 0)
//                {
//                    found = true;
//                    break;
//                }
//            }
//            if(!found){
//                Change *change = new Change(this, ChangeType::AddingAttribute, "", attributeName);
//                newChanges.push_back(change);
//            }
//        }
//    }

    for(int solutionRow = 0; solutionRow < newSolutionRowHeaders.size(); solutionRow++)
    {
        if(newSolutionRowHeaders.at(solutionRow).compare("COMMON") != 0)
        {
            bool found = false;
            QString fieldName = newSolutionRowHeaders.at(solutionRow);
            for(int changeItem = 0; changeItem < changes.size(); changeItem++)
            {
                Change *change = changes.at(changeItem);
                if(change->getType() == ChangeType::DeleteField && change->getFieldName().compare(fieldName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                Change *change = new Change(this, ChangeType::DeleteField, fieldName);
                change->setSolutionRow(solutionRow);
                newChanges.push_back(change);
            }
        }
    }

    for(int solutionColumn = 0; solutionColumn < newSolutionColumnHeaders.size(); solutionColumn++)
    {
        if(newSolutionColumnHeaders.at(solutionColumn).compare("COMMON") != 0)
        {
            bool found = false;
            QString attributeName = newSolutionColumnHeaders.at(solutionColumn);
            for(int changeItem = 0; changeItem < changes.size(); changeItem++)
            {
                Change *change = changes.at(changeItem);
                if(change->getType() == ChangeType::DeleteAttribute && change->getAttributeName().compare(attributeName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                Change *change = new Change(this, ChangeType::DeleteAttribute, "", attributeName);
                change->setSolutionColumn(solutionColumn);
                newChanges.push_back(change);
            }
        }
    }

    foreach (Change *change, newChanges)
    {
        if(change->getType() == ChangeType::DeleteField)
        {
            int solutionRow = change->getSolutionRow();
            productFile.allFields.push_back(dataTable.getAllFields()->at(solutionRow));
            QList<QString> data;
            for(int filler = 0; filler < productFile.allAttributes.size(); filler++)
            {
                data.push_back("(Not Applicable)");
            }
            for(int columnMapIndex = 0; columnMapIndex < columnMapping.size(); columnMapIndex++)
            {
                int solutionColumnIndex = columnMapping.at(columnMapIndex).at(0);
                int productColumnIndex = columnMapping.at(columnMapIndex).at(1);
                data.replace(productColumnIndex, dataTable.getRows()->at(solutionRow)->getRowCells()->at(solutionColumnIndex)->getValue());
            }
            productFile.tableData.push_back(data);
        }
    }

    foreach (Change *change, newChanges)
    {
        if(change->getType() == ChangeType::DeleteAttribute)
        {
            int solutionColumn = change->getSolutionColumn();
            productFile.allAttributes.push_back(config->columnFieldList()->at(solutionColumn)->name());
            for(int row = 0; row < productFile.allFields.size(); row++)
            {
                QList<QString> tempList = productFile.tableData.at(row);
                tempList.append("(Not Applicable)");
                productFile.tableData.replace(row, tempList);
            }
            for(int row = 0; row < productFile.allFields.size(); row++)
            {
                for(int solutionRow = 0; solutionRow < dataTable.getAllFields()->size(); solutionRow++)
                {
                    if(dataTable.getRows()->at(solutionRow)->getRowCells()->at(0)->getValue().compare(productFile.tableData.at(row).at(0)) == 0)
                    {
                        QList<QString> tempList = productFile.tableData.at(row);
                        tempList.replace(tempList.size()-1, dataTable.getRows()->at(solutionRow)->getRowCells()->at(solutionColumn)->getValue());
                        productFile.tableData.replace(row, tempList);
                        break;
                    }
                }
            }
        }
    }

    foreach (QString newAttribute, productFile.newAttributes)
    {
        productFile.allAttributes.push_back(newAttribute);
        QList<QString> newAttributeData;
        for(int row = 0; row < productFile.allFields.size(); row++)
        {
            if(row < productFile.newAttributeData.find(newAttribute).value().size())
            {
                QList<QString> tempList = productFile.tableData.at(row);
                tempList.append(productFile.newAttributeData.find(newAttribute).value().at(row));
                productFile.tableData.replace(row, tempList);
            }
            else
            {
                QList<QString> tempList = productFile.tableData.at(row);
                tempList.append("(Not Applicable)");
                productFile.tableData.replace(row, tempList);
            }
        }
    }

    QList<int> unwantedAttributes;
    for(int column = 0; column < productColumnHeaders.size(); column++)
    {
        if(!productFile.availableAttributes.contains(productFile.allAttributes.at(column)))
        {
            unwantedAttributes.push_front(column);
        }
    }
    for(int columnIndex = 0; columnIndex < unwantedAttributes.size(); columnIndex++)
    {
        for(int row = 0; row < productFile.allFields.size(); row++)
        {
            QList<QString> tempList = productFile.tableData.at(row);
            tempList.removeAt(unwantedAttributes.at(columnIndex));
            productFile.tableData.replace(row, tempList);
        }
        productFile.allAttributes.removeAt(unwantedAttributes.at(columnIndex));
    }    

    FindDiff *mergeWindow = new FindDiff(MainWindow::mainWindow, &productFile, &newChanges, config);
    mergeWindow->setModal(true);
    mergeWindow->exec();

    setNewProduct(true);
//    foreach(int newRow, newRows){
//        QMap<QString,QString> fieldData;
//        foreach (QList<int> commonColumns, columnMapping) {
//            QString columnHeader = solutionColumnHeaders.at(commonColumns.at(0));
//            QString value = productFile.tableData.at(newRow).at(commonColumns.at(1));
//            fieldData.insert(columnHeader,value);
//        }
//        QString returnValue = addFieldToModel(&fieldData);
//        if(returnValue.compare("submitted") == 0){
//            qDebug() << "Successfully added the field to the model";
//            addFieldToTableView(dataTable.getAllFields()->count()-1);
//            Change *change = new Change(this,ChangeType::AddingField,((QLineEdit*)(tableView->cellWidget(tableView->rowCount()-1,0)))->text());
//            addChange(change);
//        }else {
//            QMessageBox msg(QMessageBox::Warning, "Failed to add field", returnValue);
//            msg.exec();
//        }
//    }
//    foreach(int newColumn, newColumns){
//        QString columnHeader = productFile.newAttributes.at(newColumn);
//        AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(MainWindow::mainWindow,tableView);
//        addAttributeWindow->setFieldName(columnHeader);
//        QList<QString> fieldData;
//        for(int filler = 0; filler < dataTable.getAllFields()->size(); filler++){
//            fieldData.push_back("(Not Applicable)");
//        }
//        for(int rowMapIndex = 0; rowMapIndex < rowMapping.size(); rowMapIndex++){
//            int solutionRowIndex = rowMapping.at(rowMapIndex).at(0);
//            int productRowIndex = rowMapping.at(rowMapIndex).at(1);
//            QString productValue = productFile.newAttributeData.find(columnHeader).value().at(productRowIndex);
//            fieldData.replace(solutionRowIndex,productValue);
//        }
//        foreach(int newRow, newRows){
//            for(int fillerIndex = 0; fillerIndex < fieldData.size(); fillerIndex++){
//                QString filler = fieldData.at(fillerIndex);
//                if(filler.compare("(Not Applicable)") == 0 && productRowHeaders.at(newRow).compare(dataTable.getRows()->at(fillerIndex)->getRowCells()->at(0)->getValue()) == 0){
//                    fieldData.replace(fillerIndex, productFile.newAttributeData.find(columnHeader).value().at(newRow));
//                }
//            }
//        }
//        addAttributeWindow->setFieldData(&fieldData);
//        addAttributeWindow->setModal(true);
//        addAttributeWindow->exec();
//    }
}

void TableController::merge(QList<Change*> *approvedChanges)
{
    foreach (Change *change, *approvedChanges)
    {
        if(change->getType() == ChangeType::AddingEnum)
        {
            QString filePath = MainWindow::mainWindow->getTableControllerMap()->key(this);
            AddEnum *addEnumWindow = new AddEnum(MainWindow::mainWindow, &filePath);
            addEnumWindow->setWindowTitle("Define Enum!");
            addEnumWindow->setEnumName(change->getEnumName());
            addEnumWindow->makeNecessary();
            addEnumWindow->makeUnclosable();
            addEnumWindow->setModal(true);
            addEnumWindow->exec();
            this->createTableView();
        }
    }
    foreach (Change *change, *approvedChanges)
    {
        if(change->getType() == ChangeType::UpdateValue)
        {
            QString fieldName = change->getFieldName();
            QString attributeName = change->getAttributeName();
            QString value = change->getValue();
            int attributeIndex = config->posForColumnName(attributeName);
            FieldInfo *field = config->columnFieldList()->at(attributeIndex);
            int nameAttributeIndex = config->posForColumnName("name");
            int fieldIndex;
            for(int row = 0; row < dataTable.getRows()->size(); row++)
            {
                if(dataTable.getRows()->at(row)->getRowCells()->at(nameAttributeIndex)->getValue().compare(fieldName) == 0)
                {
                    fieldIndex = row;
                    break;
                }
            }
            if(field->fieldType() == FieldType::Text)
            {
                ((QLineEdit*)(tableView->cellWidget(fieldIndex,attributeIndex)))->setText(value);
            }
            if(field->fieldType() == FieldType::Integer)
            {
                ((QSpinBox*)(tableView->cellWidget(fieldIndex,attributeIndex)))->setValue(value.toInt());
            }
            if(field->fieldType() == FieldType::DropDown)
            {
                ((QComboBox*)(tableView->cellWidget(fieldIndex,attributeIndex)))->setCurrentText(value);
            }
            if(field->fieldType() == FieldType::Bool)
            {
                if(value.compare("true") == 0)
                {
                    ((QCheckBox*)(tableView->cellWidget(fieldIndex,attributeIndex)))->setChecked(true);
                }
                else
                {
                    ((QCheckBox*)(tableView->cellWidget(fieldIndex,attributeIndex)))->setChecked(false);
                }

            }
        }
        if(change->getType() == ChangeType::DeleteField)
        {
            QString fieldName = change->getFieldName();
            int nameAttributeIndex = config->posForColumnName("name");
            int fieldIndex;
            for(int row = 0; row < dataTable.getRows()->size(); row++)
            {
                if(dataTable.getRows()->at(row)->getRowCells()->at(nameAttributeIndex)->getValue().compare(fieldName) == 0)
                {
                    fieldIndex = row;
                    break;
                }
            }
            deleteRow(fieldIndex);
        }
        if(change->getType() == ChangeType::DeleteAttribute)
        {
            QString attributeName = change->getAttributeName();
            int attributeIndex = config->posForColumnName(attributeName);
            deleteColumn(attributeIndex);
        }
        if(change->getType() == ChangeType::AddingField)
        {
            QString returnValue = addFieldToModel(change->getUpdateData());
            if(returnValue.compare("submitted") == 0){
                addFieldToTableView(dataTable.getAllFields()->size()-1);
            }
        }
    }
    foreach (Change *change, *approvedChanges)
    {
        if(change->getType() == ChangeType::AddingAttribute)
        {
            QString fieldName = change->getAttributeName();
            int nameAttributeIndex = config->posForColumnName("name");
            AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(MainWindow::mainWindow,tableView);
            addAttributeWindow->setFieldName(fieldName);
            QList<QString> fieldData;
            for(int row = 0; row < dataTable.getAllFields()->size(); row++)
            {
                bool found = false;
                QMap<QString,QString>::iterator updateDataItr = change->getUpdateData()->begin();
                while(updateDataItr != change->getUpdateData()->end())
                {
                    if(dataTable.getRows()->at(row)->getRowCells()->at(nameAttributeIndex)->getValue().compare(updateDataItr.key()) == 0)
                    {
                        fieldData.push_back(updateDataItr.value());
                        found = true;
                    }
                    updateDataItr++;
                }
                if(!found){
                    fieldData.push_back("(Not Applicable)");
                }
            }
            addAttributeWindow->setFieldData(&fieldData);
            addAttributeWindow->setModal(true);
            addAttributeWindow->exec();
        }
    }
}

QString TableController::updateMetaData(QList<QList<QList<QString>>> *updatedMetaData)
{
    QList<QList<QString>> newCacheData;
    for(int item = 0; item < metaCacheData.size(); item++)
    {
        for(int updatedItem = 0; updatedItem < updatedMetaData->at(0).size(); updatedItem++)
        {
            if(metaCacheData.at(item).at(0).compare(updatedMetaData->at(0).at(updatedItem).at(0)) == 0)
            {
                newCacheData.append(updatedMetaData->at(0).at(updatedItem));
            }
        }
    }
    this->setMetaCacheData(&newCacheData);
    QList<QList<QString>> newMessageData;
    for(int item = 0; item < metaMessageData.size(); item++)
    {
        for(int updatedItem = 0; updatedItem < updatedMetaData->at(1).size(); updatedItem++)
        {
            if(metaMessageData.at(item).at(0).compare(updatedMetaData->at(1).at(updatedItem).at(0)) == 0)
            {
                newMessageData.append(updatedMetaData->at(1).at(updatedItem));
            }
        }
    }
    this->setMetaMessageData(&newMessageData);
    QList<QList<QString>> newSortOrder;
    newSortOrder = updatedMetaData->at(2);
    this->setDefaultSortList(&newSortOrder);
    return QString("submitted");
}

void TableController::deleteRow(int row)
{
    QString rowHeader = this->tableView->verticalHeaderItem(row)->text();
    for(int i = 0; i < dataTable.getAllFields()->size(); i++)
    {
        if(dataTable.getAllFields()->at(i).compare(rowHeader) == 0)
        {
            QString fieldName = dataTable.getRows()->at(i)->getRowCells()->at(0)->getValue();
            Change *change = new Change(this, ChangeType::AddingField, fieldName);
            changes.push_back(change);
            dataTable.getRows()->removeAt(i);
            dataTable.removeField(i);
            qDebug() << "removed " + rowHeader;
            break;
        }
    }
    for(int j = row; j < tableView->rowCount(); j++)
    {
        for(int k = 0; k < tableView->columnCount(); k++)
        {
            tableView->cellWidget(j,k)->setProperty("row",tableView->cellWidget(j,k)->property("row").toInt() - 1);
        }
    }
    tableView->model()->removeRow(row);    
    tableView->resizeColumnsToContents();
}

void TableController::deleteColumn(int column)
{
    QString columnHeader = this->tableView->horizontalHeaderItem(column)->text();
    for(int i = 0; i < dataTable.getAllAttributes()->size(); i++)
    {
        if(dataTable.getAllAttributes()->at(i).compare(columnHeader) == 0)
        {
            dataTable.removeAttribute(i);
            for(int j = 0; j < dataTable.getRows()->size(); j++)
            {
                dataTable.getRows()->at(j)->getRowCells()->removeAt(i);
                qDebug() << "removed " + columnHeader + " from " + dataTable.getAllFields()->at(j);
            }
            break;
        }
    }
    for(int j = column; j < tableView->columnCount(); j++)
    {
        for(int k = 0; k < tableView->rowCount(); k++)
        {
            tableView->cellWidget(k,j)->setProperty("column",tableView->cellWidget(k,j)->property("column").toInt() - 1);
        }
    }
    tableView->model()->removeColumn(column);
    for(int field = 0; field < config->columnFieldList()->size(); field++)
    {
        if(config->columnFieldList()->at(field)->displayName().compare(columnHeader) == 0)
        {
            FieldInfo *fieldObj = config->columnFieldList()->at(field);
            Change *change = new Change(this, ChangeType::AddingAttribute, "", fieldObj->name());
            changes.push_back(change);
            config->columnFieldList()->removeAt(field);
            qDebug() << "removed " + fieldObj->name() + " column from cache config.";
            break;
        }
    }
}

void TableController::updateTableModel(QWidget *sender)
{
    qDebug() << "updating table model";
    int row;
    int column;
    QString value;
    int fieldType = static_cast<FieldType>(sender->property("type").toInt());
    QString errorString;
    if(fieldType == 0)
    {
        QLineEdit *valueLineEdit = (QLineEdit*)sender;
        row = valueLineEdit->property("row").toInt();
        column = valueLineEdit->property("column").toInt();
        value = valueLineEdit->text();
        errorString = dataTable.updateTableModel(row, column, value);
    }
    else if(fieldType == 1)
    {
        QComboBox *valueComboBox = (QComboBox*)sender;
        row = valueComboBox->property("row").toInt();
        column = valueComboBox->property("column").toInt();
        value = valueComboBox->currentText();
        errorString = dataTable.updateTableModel(row, column, value);
    }
    else if(fieldType == 2)
    {
        QCheckBox *valueCheckBox = (QCheckBox*)sender;
        row = valueCheckBox->property("row").toInt();
        column = valueCheckBox->property("column").toInt();
        if(valueCheckBox->checkState() == Qt::PartiallyChecked)
        {
            value = "(Not Applicable)";
            errorString = dataTable.updateTableModel(row, column, value);
        }
        else if(valueCheckBox->checkState() == Qt::Checked)
        {
            value = "true";
            errorString = dataTable.updateTableModel(row, column, value);
        }
        else
        {
            value = "false";
            errorString = dataTable.updateTableModel(row, column, value);
        }
    }
    else if(fieldType == 3)
    {
        QSpinBox *valueSpinBox = (QSpinBox*)sender;
        row = valueSpinBox->property("row").toInt();
        column = valueSpinBox->property("column").toInt();
        if(valueSpinBox->specialValueText().compare("") != 0 && valueSpinBox->value() == valueSpinBox->minimum())
        {
            value = "(Not Applicable)";
            errorString = dataTable.updateTableModel(row, column, value);
        }
        else
        {
            value = QString::number(valueSpinBox->value());
            errorString = dataTable.updateTableModel(row, column, value);
        }
    }
    if(errorString.compare("submitted") != 0)
    {
        if(fieldType == 0)
        {
            ((QLineEdit*)sender)->selectAll();
            ((QLineEdit*)sender)->setText(dataTable.getRows()->at(row)->getRowCells()->at(column)->getValue());
        }
        QMessageBox msg(QMessageBox::Warning, "Update Failed", errorString);
        msg.exec();
    }
    else
    {
        QString validatedValue;
        if(fieldType == 0)
        {
            validatedValue = dataTable.getRows()->at(row)->getRowCells()->at(column)->getValue();
            ((QLineEdit*)sender)->setText(validatedValue);
        }
        if(fieldType == 3)
        {
            validatedValue = dataTable.getRows()->at(row)->getRowCells()->at(column)->getValue();
            ((QSpinBox*)sender)->setValue(validatedValue.toInt());
        }
        if(dataTable.getAllAttributes()->at(column).compare("Display Name") == 0)
        {
            dataTable.getAllFields()->replace(row, validatedValue);
            tableView->setVerticalHeaderLabels(*(dataTable.getAllFields()));
        }

        QString fieldName = ((QLineEdit*)(tableView->cellWidget(row,0)))->text();
        FieldInfo *columnField;
        foreach (FieldInfo *fieldItr, *(config->columnFieldList()))
        {
            if(fieldItr->displayName().compare(dataTable.getAllAttributes()->at(sender->property("column").toInt())) == 0)
            {
                columnField = fieldItr;
            }
        }
        int productColumn = productFile.allAttributes.indexOf(columnField->name());
        int productRow = -1;
        for(int rowItr = 0; rowItr < productFile.allFields.size(); rowItr++)
        {
            if(productFile.tableData.at(rowItr).at(0).compare(fieldName) == 0)
            {
                productRow = rowItr;
            }
        }

        bool found = false;
        for(int changeItem = 0; changeItem < changes.size(); changeItem++)
        {
            Change *change = changes.at(changeItem);
            if(change->getType() == ChangeType::UpdateValue && change->getFieldName().compare(fieldName) == 0 && change->getAttributeName().compare(columnField->name()) == 0)
            {
                change->setValue(validatedValue);
                if(productRow >= 0 && productColumn >= 0)
                {
                    change->setOldValue(productFile.tableData.at(productRow).at(productColumn));
                }
                else
                {
                    change->setOldValue("");
                }
                changes.removeAt(changeItem);
                changes.push_back(change);
                found = true;
                break;
            }
        }
        if(!found)
        {
            Change *change = new Change(this, ChangeType::UpdateValue, fieldName, columnField->name(), validatedValue);
            if(productRow >= 0 && productColumn >= 0)
            {
                change->setOldValue(productFile.tableData.at(productRow).at(productColumn));
            }
            else
            {
                change->setOldValue("");
            }
            changes.push_back(change);
        }
    }
}

bool TableController::populateTableModel(XMLFileData *data)
{
    Table *table = this->getDataTable();
    table->setAvailableAttributes(&(data->availableAttributes));
    setProductFile(data->productFile);
    version = data->versions;
    foreach(QString rowHeader , data->allFields)
    {
        table->addField(rowHeader);
    }
    foreach(QString columnHeader , data->allAttributes)
    {
        foreach(FieldInfo *field , *(config->columnFieldList())){
            if(field->name().compare(columnHeader) == 0){
                table->addAttribute(field->displayName());
            }
        }
    }
    for(int row=0; row<data->tableData.count(); row++)
    {
        for(int column=0; column<data->tableData.at(row).count(); column++)
        {
            QString attributeName = data->allAttributes.at(column);
            FieldInfo *attributeInfo = config->columnFieldList()->at(config->posForColumnName(attributeName));
            if(attributeInfo)
            {
                QString value = data->tableData.at(row).at(column);
                table->addCell(row,column,value,attributeInfo);
            }
            else
                qDebug() << "Error FieldInfo null" ;
        }

    }
    this->setMetaCacheData(&(data->metaCacheData));
    this->setMetaMessageData(&(data->metaMessageData));
    this->setDefaultSortList(&(data->defaultSortList));
    bool stopped = false;
    if(table->getNewEnums()->size() > 0)
    {
        int noOfEnums = table->getNewEnums()->size();
        for( int newEnum = 0; newEnum < noOfEnums; newEnum++)
        {
            AddEnum *addEnumWindow = new AddEnum(MainWindow::mainWindow, &(data->filePath));
            addEnumWindow->setWindowTitle("New Enum Found!");
            addEnumWindow->setEnumName(table->getNewEnums()->at(0));
            addEnumWindow->makeNecessary();
            addEnumWindow->setModal(true);
            int result = addEnumWindow->exec();
            if(result == 0)
            {
                QMessageBox::StandardButton confirmation;
                confirmation = QMessageBox::question(MainWindow::mainWindow, "Close", "Are you sure?  '" + data->fileName + "' will not be loaded without enum values!", QMessageBox::Yes|QMessageBox::No);
                if (confirmation == QMessageBox::Yes)
                {
                    stopped = true;
                    break;
                }
                else
                {
                    newEnum--;
                }
            }
            if(result == 1)
            {
                table->removeEnum(0);
            }
        }
    }
    return stopped;
}
void TableController::createTableView()
{
    this->tableView->disconnect();
    QIcon *warningIcon = MainWindow::mainWindow->getWarningIcon();
    Table *table = this->getDataTable();
    QTableWidget *tableWidget = this->getTableView();
    tableWidget->setProperty("tableController",qVariantFromValue((TableController*)this));
    tableWidget->verticalHeader()->setSectionsMovable(true);
    tableWidget->verticalHeader()->setDragEnabled(true);
    tableWidget->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setRowCount(table->getAllFields()->size());
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    QObject::connect(tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), MainWindow::mainWindow, SLOT(setSelectedColumn(int)));
    QObject::connect(tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)),  MainWindow::mainWindow, SLOT(setSelectedRow(int)));
    for ( QList<TableRow*>::iterator tableRow = table->getRows()->begin(); tableRow != table->getRows()->end(); ++tableRow)
    {
        QString errorString;
        int row = tableRow - table->getRows()->begin();
        for ( QList<TableCell*>::iterator rowCell = (*tableRow)->getRowCells()->begin(); rowCell != (*tableRow)->getRowCells()->end(); ++rowCell)
        {
            int column = rowCell - (*tableRow)->getRowCells()->begin();
            addCellToTableView(row, column, (*rowCell)->getValue(), (*rowCell)->getField(), tableWidget);
            if(table->getErrorsMap()->contains(*rowCell))
            {
                errorString.append(table->getErrorsMap()->find(*rowCell).value());
            }
        }
        if(errorString.length() > 0)
        {
            QTableWidgetItem* warningItem = new QTableWidgetItem();
            warningItem->setToolTip(errorString);
            warningItem->setIcon(*warningIcon);
            tableWidget->setVerticalHeaderItem(row,warningItem);
        }
    }
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    tableWidget->setVerticalHeaderLabels(*(table->getAllFields()));
    tableView->resizeColumnsToContents();
}

void TableController::createTreeView()
{
    versionTree->clear();
    QTreeWidgetItem *firstCommit = new QTreeWidgetItem(versionTree,QStringList(version->getVersionID()));
    firstCommit->setText(1,version->getCommitMessage());
    firstCommit->setData(0,Qt::UserRole,QVariant(version->getCommitID()));
    if (version->getCommitID() == commitID)
    {
        firstCommit->setIcon(1,QIcon(":/icons/arrow-png-22.png"));
    }

    connect(versionTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), MainWindow::mainWindow, SLOT(versionChanged(QTreeWidgetItem*,int)));
    versionTree->addTopLevelItem(firstCommit);
    foreach (Version *subVersion, *(version->getSubVersions()))
    {
        addChildToTree(firstCommit,subVersion);
    }
    versionTree->expandAll();
    versionTree->resizeColumnToContents(0);
    versionTree->resizeColumnToContents(1);
}

void TableController::addChildToTree(QTreeWidgetItem *parent, Version *subVersion)
{
    QTreeWidgetItem *child = new QTreeWidgetItem(parent);
    child->setData(0,Qt::UserRole,QVariant(subVersion->getCommitID()));
    child->setText(0,subVersion->getVersionID());
    child->setText(1,subVersion->getCommitMessage());
    if (commitID == subVersion->getCommitID())
    {
        child->setIcon(1,QIcon(":/icons/arrow-png-22.png"));
    }
    parent->addChild(child);
    foreach (Version *childVesion, *(subVersion->getSubVersions()))
    {
        addChildToTree(child, childVesion);
    }
}

void TableController::addAttributeToTableView(FieldInfo *newField, QList<QString> *fieldData)
{
    QTableWidget *tableWidget = this->getTableView();
    Table *table = this->getDataTable();
    tableWidget->setColumnCount(table->getAllAttributes()->size());
    tableWidget->setHorizontalHeaderLabels(*(table->getAllAttributes()));
    int column = table->getAllAttributes()->count()-1;
    if(!fieldData)
    {
        for(int row=0; row<table->getAllFields()->count(); row++)
        {
            addCellToTableView(row, column, newField->defaultVal(), newField, tableWidget);
        }
    }
    else
    {
        for(int row=0; row<table->getAllFields()->count(); row++)
        {
            addCellToTableView(row, column, fieldData->at(row), newField, tableWidget);
        }
    }
    tableView->resizeColumnsToContents();
}

void TableController::addFieldToTableView(int row)
{
    QTableWidget *tableWidget = this->getTableView();
    Table *table = this->getDataTable();
    tableWidget->setRowCount(table->getAllFields()->count());
    tableWidget->setVerticalHeaderLabels(*(table->getAllFields()));
    for(int column=0; column<table->getAllAttributes()->count(); column++)
    {
        TableCell *cell = table->getRows()->at(row)->getRowCells()->at(column);
        addCellToTableView(row, column, cell->getValue(), cell->getField(), tableWidget);
    }
    tableView->resizeColumnsToContents();
}

void TableController::addCellToTableView(int row, int column, QString value, FieldInfo *field, QTableWidget *tableWidget)
{
    QSignalMapper *signalMapper = new QSignalMapper(MainWindow::mainWindow);
    if(field->fieldType() == FieldType::DropDown)
    {
        QComboBox *comboBox = new QComboBox(tableWidget);
        comboBox->setProperty("type",FieldType::DropDown);
        comboBox->setProperty("row",row);
        comboBox->setProperty("column",column);
        comboBox->setFocusPolicy(Qt::ClickFocus);
        int currentIndex = 0;
        int indexCounter = 0;
        QMap<QString,QString> *valueSet = field->dropDownValMap();
        foreach(QString key , valueSet->keys())
        {
            comboBox->addItem(key);
            if(key.compare(value) == 0)
            {
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        if(field->nullable())
        {
            comboBox->addItem("");
            if(value.compare("") == 0)
            {
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        if(!field->isMandetory())
        {
            comboBox->addItem("(Not Applicable)");
            if(value.compare("(Not Applicable)") == 0)
            {
                currentIndex = indexCounter;
            }
            indexCounter++;
        }
        comboBox->setCurrentIndex(currentIndex);
        tableWidget->setCellWidget(row, column, comboBox);
        signalMapper->setMapping(comboBox,comboBox);
        QObject::connect(comboBox,SIGNAL(currentIndexChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }
    else if(field->fieldType() == FieldType::Bool)
    {
        QWidget *pWidget = new QWidget(tableWidget);
        QHBoxLayout *layout = new QHBoxLayout(pWidget);
        QCheckBox *checkBox = new QCheckBox(pWidget);
        if(!field->isMandetory())
        {
            checkBox->setTristate(true);
        }
        checkBox->setProperty("type",FieldType::Bool);
        checkBox->setProperty("row",row);
        checkBox->setProperty("column",column);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(layout);
        if(value.compare(QString("true")) == 0)
            checkBox->setChecked(true);
        else if(value.compare(QString("(Not Applicable)")) == 0)
            checkBox->setCheckState(Qt::PartiallyChecked);
        tableWidget->setCellWidget(row, column, pWidget);
        signalMapper->setMapping(checkBox,checkBox);
        QObject::connect(checkBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }
    else if(field->fieldType() == FieldType::Integer)
    {
        QSpinBox *spinBox = new QSpinBox(tableWidget);
        spinBox->setProperty("type",FieldType::Integer);
        spinBox->setProperty("row",row);
        spinBox->setProperty("column",column);
        spinBox->setMaximum(field->maxRange());
        spinBox->setMinimum(field->minRange());
        if(!field->isMandetory())
        {
            spinBox->setMinimum(field->minRange()-1);
            spinBox->setSpecialValueText("(Not Applicable)");
        }
        if(value.compare("(Not Applicable)") == 0)
        {
            spinBox->setValue(field->minRange()-1);
        }
        else
        {
            spinBox->setValue(value.toInt());
        }
        tableWidget->setCellWidget(row, column,spinBox);
        signalMapper->setMapping(spinBox,spinBox);
        QObject::connect(spinBox,SIGNAL(valueChanged(int)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }
    else if(field->fieldType() == FieldType::Text)
    {
        QLineEdit *lineEdit = new QLineEdit(tableWidget);
        lineEdit->setProperty("type",FieldType::Text);
        lineEdit->setProperty("row",row);
        lineEdit->setProperty("column",column);
        lineEdit->setText(value);
        lineEdit->setFixedWidth(175);
        tableWidget->setCellWidget(row, column,lineEdit);
        signalMapper->setMapping(lineEdit,lineEdit);
        if(field->name().compare("name") == 0)
        {
            lineEdit->setReadOnly(true);
        }
        QObject::connect(lineEdit,SIGNAL(textChanged(QString)),signalMapper,SLOT(map()));
        QObject::connect(signalMapper, SIGNAL(mapped(QWidget*)),this, SLOT(updateTableModel(QWidget*)));
    }
}

QString TableController::addAttributeToModel(FieldInfo *newField)
{
    QString returnValue = this->getDataTable()->addAttributeToModel(newField, config);
    if(returnValue.compare("submitted") == 0)
    {
        QString attrbuteName = config->columnFieldList()->at(config->columnFieldList()->size()-1)->name();
        Change *change = new Change(this, ChangeType::DeleteAttribute, "", attrbuteName);
        changes.push_back(change);
    }
    return returnValue;
}

QString TableController::addFieldToModel(QMap<QString,QString> *fieldData)
{
    QString returnValue = this->getDataTable()->addFieldToModel(fieldData, config);
    if(returnValue.compare("submitted") == 0)
    {
        QString fieldName = dataTable.getRows()->at(dataTable.getRows()->size()-1)->getRowCells()->at(0)->getValue();
        Change *change = new Change(this, ChangeType::DeleteField, fieldName);
        changes.push_back(change);
    }
    return returnValue;
}

void TableController::addChange(Change *change)
{
    this->changes.push_back(change);
}

void TableController::addNewAttributes(XMLFileData *xmlData)
{
    foreach(QString newAttribute, xmlData->newAttributes)
    {
        QString columnHeader = newAttribute;
        QMessageBox::StandardButton confirmation;
        confirmation = QMessageBox::question(MainWindow::mainWindow, "New Field Found", "Add '" + columnHeader + "' field to the solution?" , QMessageBox::Yes|QMessageBox::No);
        if (confirmation == QMessageBox::Yes)
        {
            AddAttributeWindow *addAttributeWindow = new AddAttributeWindow(MainWindow::mainWindow,tableView);
            addAttributeWindow->setFieldName(columnHeader);
            QList<QString> fieldData = xmlData->newAttributeData.find(columnHeader).value();
            addAttributeWindow->setFieldData(&fieldData);
            addAttributeWindow->setModal(true);
            addAttributeWindow->exec();
        }
        else
        {
        }
    }
}
