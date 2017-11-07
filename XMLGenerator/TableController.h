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
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <QMessageBox>

class TableController : public QObject
{
    Q_OBJECT

    public:
        explicit TableController(QWidget *tableWidgetParent = 0);
        ~TableController();

        Table* getDataTable();
        QTableWidget* getTableView();
        void populateTableModel(XMLFileData *data);
        void createTableView();
        void addAttributeToTableView(FieldInfo *newField);
        QString addAttributeToModel(FieldInfo *newField);
        QString addFieldToModel(QMap<QString,QString> *fieldData);
        void addFieldToTableView(int row);
        void addCellToTableView(int row, int column, QString value, FieldInfo *field, QTableWidget *tableWidget);
        QString updateMetaData(QList<QList<QList<QString>>> *updatedMetaData);
//        QString getColorCode(QString value, FieldInfo *field);
        QList<QList<QString>>* getMetaCacheData();
        QList<QList<QString>>* getMetaMessageData();
        QList<QList<QString>>* getDefaultSortList();

        void setMetaCacheData(QList<QList<QString>>* metaCacheData);
        void setMetaMessageData(QList<QList<QString>>* metaMessageData);
        void setDefaultSortList(QList<QList<QString>>* defaultSortList);


        void deleteRow(int row);
        void deleteColumn(int column);

    public slots:
        void updateTableModel(QWidget *sender);

    private:
        Table dataTable;
        QTableWidget *tableView;
        QList<QList<QString>> metaCacheData;
        QList<QList<QString>> metaMessageData;
        QList<QList<QString>> defaultSortList;
//        int tabIndex;
};
#endif // TABLECONTROLLER_H
