#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H
#include <Table.h>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <iostream>
#include <QDebug>
#include <QObject>
#include <Change.h>
#include <Enum.h>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QDirIterator>
#include <QFile>
#include <Version.h>

struct XMLFileData
{
    QString fileName;
    QString filePath;
    QList<QString> allFields;
    QList<QString> allAttributes;
    QList<QList<QString>> tableData;
    QList<QList<QString>> metaCacheData;
    QList<QList<QString>> metaMessageData;
    QList<QList<QString>> defaultSortList;
    QList<QList<QString>> changes;
    QList<QString> newAttributes;
    QMap<QString,QList<QString>> newAttributeData;
    CacheConfig *config;
    bool newProject;
    QString commitID;
    QString commitMessage;
    QList<QString> availableAttributes;
    XMLFileData *productFile;
    Version *versions;
};


class MainWindow;

class FileController
{
    public:
        FileController();
        ~FileController();

        QMap<int, QList<QString>>* loadFieldValues(QList<QString> *allAttributes);
        void updateFieldValues(QString attributeName,QList<QString> valueSet);
        void addProduct();
        void saveNewFields(QList<FieldInfo*> fields, QString path, QString enumsPath);
        void saveEnum(Enum *enumObject, QString path);

        QString saveXML(XMLFileData xmlData, XMLFileData productXMLData, bool newProject);
        void saveChanges(QList<Change*> *changesHistory, QString path);
        QString commit(QString path, QString commitID, bool newProduct, int commitNumber, QString *commitMessage);

        QList<Version*> loadVersions(QString filePath);

        XMLFileData* changeVersion(QString oldVersionPath, QString newVersionPath);

    public slots:
        XMLFileData* loadFile(QString *pathString = 0, bool findProduct = false);
        QList<QString> save(XMLFileData xmlData, XMLFileData productXMLData, QList<Change*> *changesHistory, QList<FieldInfo*> *fields, QList<Enum*> *newEnums, bool newProject, bool newProduct, int commitNumber, int doCommit, QString *commitMessage = 0);
    private:
};
#endif // FILECONTROLLER_H

