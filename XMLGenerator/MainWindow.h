#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <iostream>
#include <QDebug>
#include <Table.h>
#include <FileController.h>
#include <QTableWidget>
#include <QVariant>
#include <CacheConfig.h>
#include <AddEnum.h>
#include <QTreeWidgetItem>
#include <CommitWindow.h>
#include <QMovie>
#include <QTreeWidgetItemIterator>

class Table;
class TableController;
class FileController;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* mainWindow;

    void addTableController(XMLFileData *xmlData, int tabIndex);

    void setCommitMessage(QString message);

    Table* getTableModel(QTableWidget *tableWidget);
    FileController* getFileController();
    QMap<QString, TableController*>* getTableControllerMap();
    TableController* getTableController(QString key);
    QIcon* getWarningIcon();

    void saveEnum(Enum *newEnum, QString *filePath);

public slots:
    void loadFile(QString *filePath = 0, int tabIndex = -1);
    void addField();
    void addAttribute();
    void setSelectedRow(int row);
    void setSelectedColumn(int column);
    void deleteRow();
    void deleteColumn();
    void tabChanged();
    void save(int commit = 0);
    void saveAndCommit();
    void updateMetaData();
    void closeTab(QWidget *sender);
    void addProduct();
    void addEnum();
    void versionChanged(QTreeWidgetItem* sender, int column);

private:
    Ui::MainWindow *ui;
    FileController fileController;
    QMap<QString, TableController*> tableControllerMap;
    QIcon warningIcon;
    int selectedRow;
    int selectedColumn;
    QSignalMapper signalMapper;
    QString commitMessage;

//    QList<QString> allProperties;
//    QMap<QString, QList<QString>> XMLData = QMap<QString, QList<QString>>();
};
