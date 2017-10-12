#ifndef TABLECONTROLLER_H
#define TABLECONTROLLER_H
#include <Table.h>
#include <MainWindow.h>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <iostream>
#include <QDebug>
#include <QObject>
#include <QtWidgets/QTableWidget>
#include <QGridLayout>
#include <QTextCodec>
#include <QCoreApplication>
#include <QComboBox>

class TableController
{
    public:
        TableController();
        ~TableController();

        Table* getDataTable(){return &dataTable;}
        QTableWidget* getTableView(){return &tableView;}
//        int getTabIndex(){return tabIndex;}
//        QString getFilePath(){return filePath;}

//        void setTabIndex(int index){ this->tabIndex = index;}
//        void setFilePath(QString filePath){ this->filePath = filePath;}

//        void createTable(QList<QList<QString>,QList<QString>,QList<QList<QString>>> XMLData);
        void populateTableModel(XMLFileData *data,QList<FieldInfo*> *columnFieldInfo);
        QTableWidget* createTableView(QString fileName);
        void addAttributeToTableView(QString attributeName, QList<QString> valueSpace);
        QMap<QString,QList<QString>>* addAttributeToModel(QMap<QString,QString> *attributeData);
        QMap<QString,QString>* addFieldToModel(QMap<QString,QString> *fieldData,QMap<int, QList<QString>> *fieldValues);
        void addFieldToTableView(QString *fieldName, QMap<QString,QString> *fieldData, QMap<int,QList<QString>> *fieldValues);

    public slots:
    private:
        Table dataTable;
        QTableWidget tableView;
//        int tabIndex;
};
#endif // TABLECONTROLLER_H
