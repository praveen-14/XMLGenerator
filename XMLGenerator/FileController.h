#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H
#include <Table.h>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <iostream>
#include <QDebug>
#include <QObject>

struct XMLFileData
{
    QString fileName;
    QString filePath;
    QList<QString> allFields;
    QList<QString> allAttributes;
    QList<QList<QString>> tableData;
//    QMap<QString,QList<QString>> valueSets;
};


class MainWindow;

class FileController
{
    public:
        FileController();
        ~FileController();
        QMap<int, QList<QString>>* loadFieldValues(QList<QString> *allAttributes);
        void updateFieldValues(QString attributeName,QList<QString> valueSet);

    public slots:
        XMLFileData* loadFile();
    private:
};
#endif // FILECONTROLLER_H

