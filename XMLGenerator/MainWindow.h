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
    static void createMainWindow();

    void addTableController(XMLFileData *xmlData);

    Table* getTableModel(QTableWidget *tableWidget);
    FileController* getFileController(){return &fileController;}
    QMap<QString, TableController*>* getTableControllerMap(){ return &tableControllerMap;}
    TableController* getTableController(QString key);
    Ui::MainWindow* getMainUI(){return ui;}


//    void clearAllProperties(){this->allProperties.clear();}
//    void addToProperties(QString property){this->allProperties.push_back(property);}
//    QList<QString> getAllProperties(){return this->allProperties;}

//    void clearXMLData(){this->XMLData.clear();}
//    void addToXMLData(QString key, QString value)
//    {
//        if(this->XMLData.contains(key) == false){
//            QList<QString> attributeData;
//            this->XMLData.insert(key,attributeData);
//        }
//        this->XMLData[key] << value;

//    }
//    QMap<QString, QList<QString>> getXMLData(){return this->XMLData;}

public slots:
    void loadFile();
    void addField();
    void addAttribute();
    void syncTableChanges();

private:
    Ui::MainWindow *ui;
    FileController fileController;
    QMap<QString, TableController*> tableControllerMap;
    CacheConfig *cacheConfig = CacheConfig::getInstance();

//    QList<QString> allProperties;
//    QMap<QString, QList<QString>> XMLData = QMap<QString, QList<QString>>();
};
