#include <FileController.h>
#include <MainWindow.h>

FileController::FileController()
{

}

FileController::~FileController()
{

}

QMap<int, QList<QString>> FileController::loadFieldValues(QList<QString> allAttributes){
    QMap<int, QList<QString>> fieldValues;
//    QString path = QCoreApplication::applicationDirPath();
//    path.append("F:\Projects\MyProjects\XMLGenerator/FieldValues.csv");
    QFile file("F:/Projects/MyProjects/XMLGenerator/FieldValues.csv");
    file.open(QIODevice::ReadOnly);
    QTextStream fileText(&file);
    while (!fileText.atEnd()) {
        QString line = fileText.readLine();
        QList<QString> lineData = line.split(",");
        int attributeIndex = allAttributes.indexOf(lineData.first());
        QString value = lineData.back();
        if(fieldValues.contains(attributeIndex)){
            QList<QString> newData = *(fieldValues.find(attributeIndex));
            newData.append(value);
            fieldValues.insert(attributeIndex,newData);
        }else{
            QList<QString> data;
            data.append(value);
            fieldValues.insert(attributeIndex,data);
        }
    }
    file.close();
    return fieldValues;
}

XMLFileData FileController::loadFile()
{
    XMLFileData allData;
    QList<QString> allAttributes;
    QList<QString> allFields;
    QList<QList<QString>> XMLData;
//    MainWindow *mainWindow = MainWindow::mainWindow->getMainUI();
    QString path = QFileDialog::getOpenFileName(MainWindow::mainWindow,QObject::tr("Open XML File"), "",QObject::tr("XML files (*.xml)"));
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QFile remoteFile(path);
    bool fileExists = QFile::exists(path);
    if (fileExists)
    {
        bool fileOpen = remoteFile.open(QIODevice::ReadOnly);
        if (fileOpen)
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
                        allFields.append(attributes.value("displayName").toString());
                        foreach(const QXmlStreamAttribute attribute, attributes)
                        {
                            if(allAttributes.contains(attribute.name().toString()) == false) {
                                allAttributes.push_back(attribute.name().toString());
                            }
                        }
                    }
                }
                reader.readNext();
            }
        }
//        noOfColumns = this->getDataTable().getAllAttributes().size();
//        ui->tableWidget->setColumnCount(noOfColumns);
//        ui->tableWidget->setHorizontalHeaderLabels(this->getDataTable().getAllAttributes());
        remoteFile.close();
        fileOpen = remoteFile.open(QIODevice::ReadOnly);
        if (fileOpen)
        {
            QXmlStreamReader reader(&remoteFile);
            while (!reader.atEnd())
            {
                if (reader.isStartElement())
                {
                    QString name = reader.name().toString();
                    if (name == "Column")
                    {
                        QList<QString> fieldData;
//                        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                        foreach(const QString attribute, allAttributes)
                        {
                            QXmlStreamAttributes attributes = reader.attributes();
                            if(attributes.hasAttribute(attribute))
                            {
                                QString value = attributes.value(attribute).toString();
                                fieldData.append(value);
                                if(allData.valueSets.find(attribute) == allData.valueSets.end()){
                                    QList<QString> valueSpace;
                                    valueSpace.append(value);
                                    allData.valueSets.insert(attribute,valueSpace);
                                }else{
                                    QList<QString> *valueSet = &allData.valueSets.find(attribute).value();
                                    if(!valueSet->contains(value)){
                                        valueSet->append(value);
                                    }
                                }
                            }else{
                                QString value = "N/A";
                                fieldData.append(value);
                                if(allData.valueSets.find(attribute) == allData.valueSets.end()){
                                    QList<QString> valueSpace;
                                    valueSpace.append(value);
                                    allData.valueSets.insert(attribute,valueSpace);
                                }else{
                                    QList<QString> *valueSet = &allData.valueSets.find(attribute).value();
                                    if(!valueSet->contains(value)){
                                        valueSet->append(value);
                                    }
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
    allData.fileName = fileName;
    allData.filePath = path;
    allData.allFields = allFields;
    allData.allAttributes = allAttributes;
    allData.tableData = XMLData;
    return allData;
}

void FileController::updateFieldValues(QString attributeName,QList<QString> valueSet){
    QFile file("F:/Projects/MyProjects/XMLGenerator/FieldValues.csv");
    if (file.open(QFile::WriteOnly | QFile::Append)){
        QTextStream stream(&file);
        foreach(QString value , valueSet){
            stream << "\n" << attributeName << "," << value;
        }
        file.close();
    }
}
