#include <FileController.h>
#include <MainWindow.h>

FileController::FileController()
{

}

FileController::~FileController()
{
     qDebug() << "file controller deleted";
}

void FileController::saveChanges(QList<Change*> *changesHistory, QString path)
{
    QFile metaFile(path);  //type||fieldname||attributeName||value||oldValue
    metaFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream textStream(&metaFile);
    foreach (Change *change, *changesHistory)
    {
        textStream << static_cast<ChangeType>(change->getType());
        textStream << QString("||").toUtf8();
        textStream << change->getFieldName().toUtf8();
        textStream << QString("||").toUtf8();
        textStream << change->getAttributeName().toUtf8();
        textStream << QString("||").toUtf8();
        textStream << change->getValue().toUtf8();
        textStream << QString("||").toUtf8();
        textStream << change->getOldValue().toUtf8();
        textStream << QString("||").toUtf8();
        textStream << endl;
    }
}

QString FileController::saveXML(XMLFileData xmlData, XMLFileData productXMLData, bool newProject)
{
    QString saveFolder;
    if(newProject)
    {
        saveFolder = QFileDialog::getExistingDirectory(MainWindow::mainWindow, QObject::tr("Select Project Path"),
                                   "C:/Users/",
                                   QFileDialog::ShowDirsOnly);
    }
    else
    {
        QFileInfo fileInfo(xmlData.filePath);
        saveFolder = fileInfo.path();
    }
    QString xmlFilePath = "";
    if(saveFolder.size() > 0)
    {
        QString xmlFolderPath;
        QString fileName = xmlData.fileName;
        if(newProject)
        {
            QString prevFileName = fileName;
            prevFileName.chop(4);
            xmlFolderPath = saveFolder + "/" + prevFileName;
            if(QFile(xmlFolderPath).exists())
            {
                QMessageBox::StandardButton confirmation;
                confirmation = QMessageBox::question(MainWindow::mainWindow, "Warning!", "Project named '" + prevFileName + "' found in the specified directory!\nReplace existing project?" , QMessageBox::Yes|QMessageBox::No);
                if (confirmation == QMessageBox::No)
                {
                    return saveXML(xmlData,productXMLData,newProject);
                }
                else
                {
                    QDir dir(xmlFolderPath);
                    dir.removeRecursively();
                }
            }
            QDir dir = QDir::root();
            dir.mkpath(xmlFolderPath);
        }
        else
        {
            xmlFolderPath = saveFolder;
        }
        xmlFilePath = xmlFolderPath + "/" + fileName;

        QFile file(xmlFilePath);
        file.open(QFile::WriteOnly);
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.setAutoFormattingIndent(4);
        xmlWriter.writeStartElement("cache");
            for(int cacheItem = 0; cacheItem < xmlData.metaCacheData.size(); cacheItem++)
            {
                xmlWriter.writeAttribute(xmlData.metaCacheData.at(cacheItem).at(0), xmlData.metaCacheData.at(cacheItem).at(1));
            }
            xmlWriter.writeStartElement("message");
                for(int messageItem = 0; messageItem < xmlData.metaMessageData.size(); messageItem++)
                {
                    xmlWriter.writeAttribute(xmlData.metaMessageData.at(messageItem).at(0), xmlData.metaMessageData.at(messageItem).at(1));
                }
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("columns");
                for(int row = 0; row < xmlData.allFields.size(); row++)
                {
                    xmlWriter.writeStartElement("Column");
                        for(int column = 0; column < xmlData.allAttributes.size(); column++)
                        {
                            if(xmlData.tableData.at(row).at(column).compare(QString("(Not Applicable)")) != 0)
                            {
                                xmlWriter.writeAttribute(xmlData.allAttributes.at(column), xmlData.tableData.at(row).at(column));
                            }
                        }
                    xmlWriter.writeEndElement();
                }
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement("DefaultSortOrder");
                for(int sortItem = 0; sortItem < xmlData.defaultSortList.size(); sortItem++)
                {
                    xmlWriter.writeStartElement("SortField");
                    xmlWriter.writeAttribute("sortColumn", xmlData.defaultSortList.at(sortItem).at(0));
                    xmlWriter.writeAttribute("sortAsc", xmlData.defaultSortList.at(sortItem).at(1));
                    xmlWriter.writeEndElement();
                }
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        fileName.chop(3);
        fileName.append("product");
        QString productPath = xmlFolderPath + "/" + fileName;

        QFile productFile(productPath);
        productFile.open(QFile::WriteOnly);
        QXmlStreamWriter productXmlWriter(&productFile);
        productXmlWriter.setAutoFormatting(true);
        productXmlWriter.setAutoFormattingIndent(4);
        productXmlWriter.writeStartElement("cache");
            for(int cacheItem = 0; cacheItem < productXMLData.metaCacheData.size(); cacheItem++)
            {
                productXmlWriter.writeAttribute(productXMLData.metaCacheData.at(cacheItem).at(0), productXMLData.metaCacheData.at(cacheItem).at(1));
            }
            productXmlWriter.writeStartElement("message");
                for(int messageItem = 0; messageItem < productXMLData.metaMessageData.size(); messageItem++)
                {
                    productXmlWriter.writeAttribute(productXMLData.metaMessageData.at(messageItem).at(0), productXMLData.metaMessageData.at(messageItem).at(1));
                }
            productXmlWriter.writeEndElement();
            productXmlWriter.writeStartElement("columns");
                for(int row = 0; row < productXMLData.allFields.size(); row++)
                {
                    productXmlWriter.writeStartElement("Column");
                        for(int column = 0; column < productXMLData.allAttributes.size(); column++)
                        {
                            if(productXMLData.tableData.at(row).at(column).compare(QString("(Not Applicable)")) != 0)
                            {
                                productXmlWriter.writeAttribute(productXMLData.allAttributes.at(column), productXMLData.tableData.at(row).at(column));
                            }
                        }
                        for(int newColumn = 0; newColumn < productXMLData.newAttributes.size(); newColumn++)
                        {
                            QString attributeName = productXMLData.newAttributes.at(newColumn);
                            if(productXMLData.newAttributeData.find(attributeName).value().at(row).compare(QString("(Not Applicable)")) != 0)
                            {
                                productXmlWriter.writeAttribute(attributeName, productXMLData.newAttributeData.find(attributeName).value().at(row));
                            }
                        }
                    productXmlWriter.writeEndElement();
                }
            productXmlWriter.writeEndElement();

            productXmlWriter.writeStartElement("DefaultSortOrder");
                for(int sortItem = 0; sortItem < productXMLData.defaultSortList.size(); sortItem++)
                {
                    productXmlWriter.writeStartElement("SortField");
                    productXmlWriter.writeAttribute("sortColumn", productXMLData.defaultSortList.at(sortItem).at(0));
                    productXmlWriter.writeAttribute("sortAsc", productXMLData.defaultSortList.at(sortItem).at(1));
                    productXmlWriter.writeEndElement();
                }
            productXmlWriter.writeEndElement();
        productXmlWriter.writeEndElement();
    }
    return xmlFilePath;
}

void FileController::saveEnum(Enum *enumObject, QString path){
    QDomDocument document("enumFile");
    QFile enumSaver(path);
    if(enumSaver.exists())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            return;
        if (!document.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
    }
    else
    {
        QFile file(QDir::currentPath() + "/Enums.spres");
        if (!file.open(QIODevice::ReadOnly))
            return;
        if (!document.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
    }
     if(enumObject)
     {
        QDomNode enums = document.documentElement();
        QDomElement newEnum = document.createElement("Enum");
        newEnum.setAttribute("name", enumObject->getName());
        newEnum.setAttribute("group", "");
        newEnum.setAttribute("defaultVal", "");
        QMap<QString,QString>::iterator enumValuesIterator = enumObject->getValues()->begin();
        while(enumValuesIterator != enumObject->getValues()->end())
        {
            QDomElement newItem = document.createElement("Item");
            newItem.setAttribute("value", enumValuesIterator.key());
            QDomText textElement = document.createTextNode(enumValuesIterator.value());
            newItem.appendChild(textElement);
            newEnum.appendChild(newItem);
            enumValuesIterator++;
        }
        enums.appendChild(newEnum);
    }

    enumSaver.open(QIODevice::WriteOnly);
    QTextStream writer(&enumSaver);
    writer << document.toString();

}

void FileController::saveNewFields(QList<FieldInfo*> fields, QString path, QString enumsPath){
    QDomDocument document("cacheXML");
    QFile file(QDir::currentPath() + "/CacheXMLConfigs.xml");
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!document.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomNode configs = document.documentElement();
    QDomNode cacheFields = configs.firstChild();
    QDomNode messageFields = cacheFields.nextSibling();
    QDomNode columnFields = messageFields.nextSibling();
    QDomNodeList columnFieldList = columnFields.childNodes();
    bool enumPathUpdated = false;
    foreach (FieldInfo *field, fields)
    {
        bool found = false;
        for(int nodeIndex = 0; nodeIndex < columnFieldList.size() ; nodeIndex++)
        {
            QDomNode node = columnFieldList.item(nodeIndex);
            QString columnName = node.toElement().attribute("name");
            if(field->name().compare(columnName) == 0)
            {
                found = true;
            }
            if(!enumPathUpdated && columnName.compare("enumName") == 0)
            {
                node.toElement().setAttribute("enumFile", enumsPath);
                enumPathUpdated = true;
            }
        }
        if(!found)
        {
            QDomElement newColumn = document.createElement("field");
            newColumn.setAttribute("name", field->name());
            newColumn.setAttribute("displayName", field->displayName());
            newColumn.setAttribute("fieldType", field->fieldType());
            if(field->isMandetory())
            {
                newColumn.setAttribute("isMandetory", "true");
            }
            else
            {
                newColumn.setAttribute("isMandetory", "false");
            }
            if(field->nullable())
            {
                newColumn.setAttribute("nullable", "true");
            }
            else
            {
                newColumn.setAttribute("nullable", "false");
            }
            if(field->dropDownValMap()->size() > 0)
            {
                QMap<QString,QString>::iterator valMapIterator = field->dropDownValMap()->begin();
                while(valMapIterator != field->dropDownValMap()->end())
                {
                    QDomElement newItem = document.createElement("item");
                    newItem.setAttribute("name", valMapIterator.key());
                    newItem.setAttribute("value", valMapIterator.value());
                    newColumn.appendChild(newItem);
                    valMapIterator++;
                }
            }
            if(field->fieldType() == FieldType::Integer)
            {
                newColumn.setAttribute("minRange", field->minRange());
                newColumn.setAttribute("maxRange", field->maxRange());
            }
            columnFields.appendChild(newColumn);
        }
    }
    QFile configSaver(path);
    configSaver.open(QIODevice::WriteOnly);
    QTextStream writer(&configSaver);
    writer << document.toString();
}

QList<QString> FileController::save(XMLFileData xmlData, XMLFileData productXMLData, QList<Change*> *changesHistory, QList<FieldInfo*> *fields, QList<Enum*> *newEnums, bool newProject, bool newProduct, int commitNumber, int doCommit, QString *commitMessage)
{
    QString savePath = saveXML(xmlData, productXMLData, newProject);

    QList<QString> returnValue;
    if(savePath.size() > 0)
    {
        QFileInfo fileInfo(savePath);
        QString fileName = fileInfo.fileName();
        fileName.chop(4);
        QString path = fileInfo.path();
        QString changesPath = path + "\\" + fileName + ".meta";
        QString enumsPath = path + "\\Enums.spres";
        QString configPath = path + "\\CacheXMLConfigs.xml";
        saveChanges(changesHistory,changesPath);
        saveNewFields(*fields,configPath,path + "/Enums.spres");
        if(newEnums->size() == 0)
        {
            this->saveEnum(nullptr, enumsPath);
        }
        else
        {
            QList<Enum*>::iterator enumObjectIterator = newEnums->begin();
            while(enumObjectIterator != newEnums->end())
            {
                this->saveEnum(*enumObjectIterator, enumsPath);
                enumObjectIterator++;
            }
        }

        QString commitID;
        if(doCommit)
        {
            commitID = commit(path, xmlData.commitID, newProduct, commitNumber, commitMessage);
        }
        returnValue.push_back(savePath);
        returnValue.push_back(commitID);
    }
    return returnValue;
}

QString FileController::commit(QString path, QString commitID, bool newProduct, int commitNumber, QString *commitMessage)
{
    QString versionFolderPath = path + "/versions";
    QFile versionFolder(versionFolderPath);
    if(!versionFolder.exists())
    {
        QDir dir = QDir::root();
        dir.mkpath(versionFolderPath);
    }

    QString commitFilePath = path + "/.commit";
    QFile commitFile(commitFilePath);
    commitFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);

    QString newCommitID;
    QTextStream textStream(&commitFile);

    QString newCommitPath;
    if(commitID.size() == 0)
    {
        newCommitID = "1.1";
        newCommitPath = newCommitID;
        QString newVersionFolder = versionFolderPath + "/" + newCommitPath;
        QDir dir = QDir::root();
        dir.mkpath(newVersionFolder);
        textStream << newCommitPath;
        textStream << endl;
        textStream << *commitMessage;
        textStream << endl;
        commitFile.close();
    }
    else
    {
        QList<QString> versionPathList = commitID.split("/");
        QList<QString> lastVersionDetails = versionPathList.at(versionPathList.size()-1).split(".");

        int productVersion = lastVersionDetails.at(0).toInt();
        int solutionVersion = commitNumber;
        if(newProduct)
        {
            productVersion++;
        }
        newCommitID = QString::number(productVersion) + "." + QString::number(solutionVersion);
        QString newCommitFolder = versionFolderPath + "/" + commitID + "/" + newCommitID;
        QDir dir = QDir::root();
        newCommitPath = commitID + "/" + newCommitID;
        dir.mkpath(newCommitFolder);
        textStream << newCommitPath;
        textStream << endl;
        textStream << *commitMessage;
        textStream << endl;
        commitFile.close();
    }

    QDirIterator *dirIterator = new QDirIterator(path,QDir::Files);
    while (dirIterator->hasNext())
    {
        QString iteratorPath = dirIterator->next();
        QFileInfo fileInfo(iteratorPath);
        QString fileName = fileInfo.fileName();
        QFile::copy(iteratorPath,versionFolderPath + "/" + newCommitPath + "/" + fileName);
    }
    return newCommitPath;
}

QList<Version*> FileController::loadVersions(QString filePath)
{
    QList<Version*> versions;
    QDirIterator *dirIterator = new QDirIterator(filePath,QDir::Dirs|QDir::NoDot|QDir::NoDotAndDotDot);
    while (dirIterator->hasNext())
    {
        QString commitID;
        QString commitMessage;
        QString iteratorPath = dirIterator->next();
        QString commitFilePath = iteratorPath + "/.commit";
        QFile commitFile(commitFilePath);
        bool commitFileExists = QFile::exists(commitFilePath);
        if(commitFileExists)
        {
            bool fileOpen = commitFile.open(QIODevice::ReadOnly);
            if (fileOpen)
            {
                QTextStream inputStream(&commitFile);
                while(!inputStream.atEnd())
                {
                    QString line = inputStream.readLine();
                    if(line.size() > 0)
                    {
                        commitID = line;
                    }
                    QString line2 = inputStream.readLine();
                    if(line2.size() > 0)
                    {
                        commitMessage = line2;
                    }
                }
                commitFile.close();
            }
        }
        Version *version = new Version(commitID);
        version->setCommitMessage(commitMessage);
        version->addSubVersions(loadVersions(iteratorPath));
        versions.push_back(version);
    }
    return versions;
}

XMLFileData* FileController::changeVersion(QString oldVersionPath, QString newVersionPath)
{
    QFileInfo oldVersion(oldVersionPath);
    QString oldFolderPath = oldVersion.path();
    QString fileName = oldVersion.fileName();

    QFileInfo newVersion(oldFolderPath + "/versions/" + newVersionPath + "/" + fileName);
    QString newFolder = newVersion.path();

    QDirIterator *oldDirIterator = new QDirIterator(oldFolderPath,QDir::Files);
    while (oldDirIterator->hasNext())
    {
        QString iteratorPath = oldDirIterator->next();
        QFile fileToRemove(iteratorPath);
        fileToRemove.remove();
    }

    QDirIterator *newDirIterator = new QDirIterator(newFolder,QDir::Files);
    while (newDirIterator->hasNext())
    {
        QString iteratorPath = newDirIterator->next();
        QFileInfo fileInfo(iteratorPath);
        QString fileName = fileInfo.fileName();
        QFile::copy(iteratorPath,oldFolderPath + "/" + fileName);
    }
}

XMLFileData* FileController::loadFile(QString *pathString, bool findProduct)
{
    XMLFileData *allData = new XMLFileData();
    QList<QString> allAttributes;
    QList<QString> allFields;
    QList<QList<QString>> XMLData;
    QList<QList<QString>> metaCacheData;
    QList<QList<QString>> metaMessageData;
    QList<QList<QString>> defaultSortList;
    QList<QList<QString>> changes;
    QList<QString> newAttributes;
    QMap<QString,QList<QString>> newAttributeData;
    bool newProject = true;
    QString commitID;
    QString commitMessage;
    QList<QString> availableAttributes;
    XMLFileData *productFile;
    Version *version = 0;

    QString path;
    if(!pathString)
    {
        path = QFileDialog::getOpenFileName(MainWindow::mainWindow,QObject::tr("Open XML File"), "",QObject::tr("XML files (*.xml)"));
    }
    else
    {
        path = *pathString;
    }


    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QFile remoteFile(path);
    bool fileExists = QFile::exists(path);
    CacheConfig *config;

    if (fileExists)
    {
        QString tempFileName = fileName;
        tempFileName.chop(4);
        if(QFile::exists(fileInfo.path() + "/" + tempFileName + ".meta"))
        {
            QString metaPath = fileInfo.path();
            metaPath.append("\\" + tempFileName + ".meta");
            QFile metaFile(metaPath);
            bool metaFileExists = QFile::exists(metaPath);
            if(metaFileExists)
            {
                bool fileOpen = metaFile.open(QIODevice::ReadOnly);
                if (fileOpen)
                {
                    QTextStream inputStream(&metaFile);
                    while(!inputStream.atEnd())
                    {
                        QString line = inputStream.readLine();
                        QList<QString> change = line.split("||");
                        changes.push_back(change);
                    }
                    metaFile.close();
                }
            }
            newProject = false;
        }

        if(QFile::exists(fileInfo.path() + "/" + tempFileName + ".product") && findProduct)
        {
            productFile = loadFile(new QString(fileInfo.path() + "/" + tempFileName + ".product") ,false);
        }
        else
        {
            productFile = allData;
        }

        if(QFile::exists(fileInfo.path() + "/.commit"))
        {
            QString commitPath = fileInfo.path();
            commitPath.append("/.commit");
            QFile commitFile(commitPath);
            bool commitFileExists = QFile::exists(commitPath);
            if(commitFileExists)
            {
                bool fileOpen = commitFile.open(QIODevice::ReadOnly);
                if (fileOpen)
                {
                    QTextStream inputStream(&commitFile);
                    while(!inputStream.atEnd())
                    {
                        QString line = inputStream.readLine();
                        if(line.size() > 0)
                        {
                            commitID = line;
                        }
                        QString line2 = inputStream.readLine();
                        if(line2.size() > 0)
                        {
                            commitMessage = line2;
                        }
                    }
                    commitFile.close();
                }
            }
        }

        config = new CacheConfig(MainWindow::mainWindow);
        if(QFile::exists(fileInfo.path() + "/CacheXMLConfigs.xml"))
        {
            qDebug() << "loading project specific config file";
            config->init(fileInfo.path() + "/CacheXMLConfigs.xml");
        }
        else
        {
            qDebug() << "loading general config file";
            config->init(QDir::currentPath() + "/CacheXMLConfigs.xml");
        }
        for(int i = 0; i < config->columnFieldList()->size(); i++)
        {
            allAttributes.push_back(config->columnFieldList()->at(i)->name());
        }

        if(QFile::exists(fileInfo.path() + "/versions"))
        {
            qDebug() << "loading versions";
            version = this->loadVersions(QString(fileInfo.path() + "/versions")).at(0);
        }

        bool fileOpenIni = remoteFile.open(QIODevice::ReadOnly);
        if (fileOpenIni)
        {
            QXmlStreamReader reader(&remoteFile);
            while (!reader.atEnd())
            {
                if (reader.isStartElement())
                {
                    QString name = reader.name().toString();
                    if (name == "Column")
                    {
                        QXmlStreamAttributes attributes = reader.attributes();
                        foreach(const QXmlStreamAttribute attribute, attributes)
                        {
                            if(allAttributes.contains(attribute.name().toString()) == false && newAttributes.contains(attribute.name().toString()) == false)
                            {
                                newAttributes.push_back(attribute.name().toString());
                            }
                            if(availableAttributes.contains(attribute.name().toString()) == false)
                            {
                                availableAttributes.push_back(attribute.name().toString());
                            }
                        }
                    }
                }
                reader.readNext();
            }
        }
        remoteFile.close();
        bool fileOpen = remoteFile.open(QIODevice::ReadOnly);
        if (fileOpen)
        {
            QXmlStreamReader reader(&remoteFile);
            while (!reader.atEnd())
            {
                if (reader.isStartElement())
                {
                    QString name = reader.name().toString();
                    if (name == "cache")
                    {
                        QXmlStreamAttributes attributes = reader.attributes();
                        foreach (QXmlStreamAttribute attribute, attributes)
                        {
                            QList<QString> cacheField;
                            cacheField.push_back(attribute.name().toString());
                            cacheField.push_back(attribute.value().toString());
                            metaCacheData.push_back(cacheField);
                        }
                    }
                    if (name == "message")
                    {
                        QXmlStreamAttributes attributes = reader.attributes();
                        foreach (QXmlStreamAttribute attribute, attributes)
                        {
                            QList<QString> messageField;
                            messageField.push_back(attribute.name().toString());
                            messageField.push_back(attribute.value().toString());
                            metaMessageData.push_back(messageField);
                        }
                    }
                    if (name == "SortField")
                    {
                        QXmlStreamAttributes attributes = reader.attributes();
                        QList<QString> sortField;
                        sortField.push_back(attributes.value("sortColumn").toString());
                        sortField.push_back(attributes.value("sortAsc").toString());
                        defaultSortList.push_back(sortField);
                    }
                    if (name == "Column")
                    {
                        QList<QString> fieldData;
                        QXmlStreamAttributes attributes = reader.attributes();
                        allFields.append(attributes.value("displayName").toString());
                        foreach(const QString attribute, allAttributes)
                        {
                            if(attributes.hasAttribute(attribute))
                            {
                                QString value = attributes.value(attribute).toString();
                                fieldData.append(value);
                            }
                            else
                            {
                                QString value = "(Not Applicable)";
                                fieldData.append(value);
                            }
                        }
                        foreach(const QString newAttribute, newAttributes)
                        {
                            if(attributes.hasAttribute(newAttribute))
                            {
                                QString value = attributes.value(newAttribute).toString();
                                if(newAttributeData.contains(newAttribute))
                                {
                                    newAttributeData.find(newAttribute).value().push_back(value);
                                }
                                else
                                {
                                    QList<QString> data;
                                    data.push_back(value);
                                    newAttributeData.insert(newAttribute,data);
                                }
                            }
                            else
                            {
                                QString value = "(Not Applicable)";
                                if(newAttributeData.contains(newAttribute))
                                {
                                    newAttributeData.find(newAttribute).value().push_back(value);
                                }
                                else
                                {
                                    QList<QString> data;
                                    data.push_back(value);
                                    newAttributeData.insert(newAttribute,data);
                                }
                            }
                        }
                        XMLData.append(fieldData);
                    }
                }
                reader.readNext();
            }
        }
        remoteFile.close();
    }



    allData->fileName = fileName;
    allData->filePath = path;
    allData->allFields = allFields;
    allData->allAttributes = allAttributes;
    allData->tableData = XMLData;
    allData->defaultSortList = defaultSortList;
    allData->metaCacheData = metaCacheData;
    allData->metaMessageData = metaMessageData;
    allData->changes = changes;
    allData->newAttributes = newAttributes;
    allData->newAttributeData = newAttributeData;
    allData->config = config;
    allData->newProject = newProject;
    allData->commitID = commitID;
    allData->commitMessage = commitMessage;
    allData->availableAttributes = availableAttributes;
    allData->productFile = productFile;
    allData->versions = version;
    if(fileExists)
    {
        return allData;
    }
    else
    {
        return nullptr;
    }
}
