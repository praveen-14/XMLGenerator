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

    void addTableController(XMLFileData *xmlData);

    Table* getTableModel(QTableWidget *tableWidget);
    FileController* getFileController();
    QMap<QString, TableController*>* getTableControllerMap();
    TableController* getTableController(QString key);
    QIcon* getWarningIcon();

public slots:
    void loadFile();
    void addField();
    void addAttribute();
    void setSelectedRow(int row);
    void setSelectedColumn(int column);
    void deleteRow();
    void deleteColumn();
    void tabChanged();
    void save();
    void updateMetaData();
    void closeTab(int index);

private:
    Ui::MainWindow *ui;
    FileController fileController;
    QMap<QString, TableController*> tableControllerMap;
    QIcon warningIcon;
    int selectedRow;
    int selectedColumn;

//    QList<QString> allProperties;
//    QMap<QString, QList<QString>> XMLData = QMap<QString, QList<QString>>();
};
