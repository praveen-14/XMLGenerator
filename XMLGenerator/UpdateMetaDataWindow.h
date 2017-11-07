#ifndef UPDATEMETADATAWINDOW_H
#define UPDATEMETADATAWINDOW_H

#include <QDialog>
#include <CacheConfig.h>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <MainWindow.h>
#include <TableController.h>

namespace Ui {
class UpdateMetaDataWindow;
}

class UpdateMetaDataWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateMetaDataWindow(QWidget *parent = 0, QList<QList<QString>> *metaCacheData = 0, QList<QList<QString>> *metaMessageData = 0, QList<QList<QString>> *defaultSortList = 0, QTableWidget *tableWidget = 0);
    ~UpdateMetaDataWindow();

    void populateWindow();
    QString getValue(FieldInfo *field, QList<QList<QString>> *list);

public slots:
    void addSortOrder();
    void removeValue(int id);
    void saveData();

private:
    Ui::UpdateMetaDataWindow *ui;
    int sortOrderID;
    QSignalMapper *signalMapper;
    QList<QList<QString>> *metaCacheData;
    QList<QList<QString>> *metaMessageData;
    QList<QList<QString>> *defaultSortList;
    QTableWidget *tableWidget;
};

#endif // UPDATEMETADATAWINDOW_H
