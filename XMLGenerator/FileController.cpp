#include <FileController.h>
#include <MainWindow.h>

FileController::FileController()
{

}

FileController::~FileController()
{
     qDebug() << "file controller deleted";
}

void FileController::save(XMLFileData xmlData){
    QString saveFile = QFileDialog::getSaveFileName(MainWindow::mainWindow, QObject::tr("Save File"),
                               "C:/Users/praveense/Desktop",
                               QObject::tr("XML files (*.xml)"));
    QFile file(saveFile);
    file.open(QFile::WriteOnly);
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(4);
    xmlWriter.writeStartElement("cache");
        for(int cacheItem = 0; cacheItem < xmlData.metaCacheData.size(); cacheItem++){
            xmlWriter.writeAttribute(xmlData.metaCacheData.at(cacheItem).at(0), xmlData.metaCacheData.at(cacheItem).at(1));
        }
        xmlWriter.writeStartElement("message");
            for(int messageItem = 0; messageItem < xmlData.metaMessageData.size(); messageItem++){
                xmlWriter.writeAttribute(xmlData.metaMessageData.at(messageItem).at(0), xmlData.metaMessageData.at(messageItem).at(1));
            }
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("columns");
            for(int row = 0; row < xmlData.allFields.size(); row++){
                xmlWriter.writeStartElement("Column");
                    for(int column = 0; column < xmlData.allAttributes.size(); column++){
                        if(xmlData.tableData.at(row).at(column).compare(QString("(Not Applicable)")) != 0){
                            xmlWriter.writeAttribute(xmlData.allAttributes.at(column), xmlData.tableData.at(row).at(column));
                        }
                    }
                xmlWriter.writeEndElement();
            }
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("DefaultSortOrder");
            for(int sortItem = 0; sortItem < xmlData.defaultSortList.size(); sortItem++){
                xmlWriter.writeStartElement("SortField");
                xmlWriter.writeAttribute("sortColumn", xmlData.defaultSortList.at(sortItem).at(0));
                xmlWriter.writeAttribute("sortAsc", xmlData.defaultSortList.at(sortItem).at(1));
                xmlWriter.writeEndElement();
            }
        xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
}

XMLFileData* FileController::loadFile()
{
    XMLFileData *allData = new XMLFileData();
    QList<QString> allAttributes;
    QList<QString> allFields;
    QList<QList<QString>> XMLData;
    QList<QList<QString>> metaCacheData;
    QList<QList<QString>> metaMessageData;
    QList<QList<QString>> defaultSortList;

    CacheConfig *config = CacheConfig::getInstance();
    for(int i = 0; i < config->columnFieldList()->size(); i++){
        allAttributes.push_back(config->columnFieldList()->at(i)->name());
    }

    QString path = QFileDialog::getOpenFileName(MainWindow::mainWindow,QObject::tr("Open XML File"), "",QObject::tr("XML files (*.xml)"));
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QFile remoteFile(path);
    bool fileExists = QFile::exists(path);
    if (fileExists)
    {
//        bool fileOpen = remoteFile.open(QIODevice::ReadOnly);
//        if (fileOpen)
//        {
//            QXmlStreamReader reader(&remoteFile);
//            while (!reader.atEnd())
//            {
//                if (reader.isStartElement())
//                {
//                    QString name = reader.name().toString();
//                    if (name == "Column")
//                    {
//                        QXmlStreamAttributes attributes = reader.attributes();
//                        allFields.append(attributes.value("displayName").toString());
//                        foreach(const QXmlStreamAttribute attribute, attributes)
//                        {
//                            if(allAttributes.contains(attribute.name().toString()) == false) {
//                                allAttributes.push_back(attribute.name().toString());
//                            }
//                        }
//                    }
//                }
//                reader.readNext();
//            }
//        }
//        remoteFile.close();
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
                        foreach (QXmlStreamAttribute attribute, attributes) {
                            QList<QString> cacheField;
                            cacheField.push_back(attribute.name().toString());
                            cacheField.push_back(attribute.value().toString());
                            metaCacheData.push_back(cacheField);
                        }
                    }
                    if (name == "message")
                    {
                        QXmlStreamAttributes attributes = reader.attributes();
                        foreach (QXmlStreamAttribute attribute, attributes) {
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
                            QXmlStreamAttributes attributes = reader.attributes();
                            if(attributes.hasAttribute(attribute))
                            {
                                QString value = attributes.value(attribute).toString();
                                fieldData.append(value);
                            }else{
                                QString value = "(Not Applicable)";
                                fieldData.append(value);
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
    if(fileExists){
        return allData;
    }else{
        return nullptr;
    }
}
