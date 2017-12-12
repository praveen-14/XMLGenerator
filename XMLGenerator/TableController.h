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
#include <Change.h>
#include <AddAttributeWindow.h>
#include <FindDiff.h>
#include <QTreeWidget>

class TableController : public QObject
{
    Q_OBJECT

    public:
        explicit TableController(QWidget *tableWidgetParent = 0, CacheConfig * config = 0, QObject *parent = 0);
        ~TableController();

        Table* getDataTable();
        QTableWidget* getTableView();
        QList<Change*>* getChanges();
        XMLFileData* getProductFile();
        XMLFileData* getUnresolvedProductFile();
        CacheConfig* getConfig();
        bool isNewProject();
        bool isNewProduct();
        QString getCommitID();
        QTreeWidget* getVersionTree();
        Version* getVersion();

        bool populateTableModel(XMLFileData *data);
        void createTableView();
        void createTreeView();
        void addChildToTree(QTreeWidgetItem *parent, Version *subVersion);
        void findDiff();
        void merge(QList<Change*> *approvedChanges);

        void addAttributeToTableView(FieldInfo *newField, QList<QString> *fieldData = 0);
        QString addAttributeToModel(FieldInfo *newField);
        QString addFieldToModel(QMap<QString,QString> *fieldData);
        void addFieldToTableView(int row);
        void addCellToTableView(int row, int column, QString value, FieldInfo *field, QTableWidget *tableWidget);
        void addNewAttributes(XMLFileData *xmlData);

        QString updateMetaData(QList<QList<QList<QString>>> *updatedMetaData);
        QList<QList<QString>>* getMetaCacheData();
        QList<QList<QString>>* getMetaMessageData();
        QList<QList<QString>>* getDefaultSortList();

        void setMetaCacheData(QList<QList<QString>>* metaCacheData);
        void setMetaMessageData(QList<QList<QString>>* metaMessageData);
        void setDefaultSortList(QList<QList<QString>>* defaultSortList);
        void setProductFile(XMLFileData *fileData);
        void setNewProject(bool newProject);
        void setNewProduct(bool newProduct);
        void setCommitID(QString commitID);
        void setVersion(Version *version);

        void deleteRow(int row);
        void deleteColumn(int column);

        void addChange(Change *change);

        void saveEnum(Enum *enumObject);
        QList<Enum*>* getNewEnums();

    public slots:
        void updateTableModel(QWidget *sender);

    private:
        Table dataTable;
        QTableWidget *tableView;
        QTreeWidget *versionTree;
        QList<QList<QString>> metaCacheData;
        QList<QList<QString>> metaMessageData;
        QList<QList<QString>> defaultSortList;
        QList<Change*> changes;
        XMLFileData productFile;
        XMLFileData unresolvedProductFile;
        QList<Enum*> newEnums;
        CacheConfig *config;
        bool newProject;
        bool newProduct;
        QString commitID;
        Version *version;
//        int tabIndex;
};
#endif // TABLECONTROLLER_H
