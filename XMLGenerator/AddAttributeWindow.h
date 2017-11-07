#ifndef ADDATTRIBUTEWINDOW_H
#define ADDATTRIBUTEWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QScrollArea>
#include <QCheckBox>
#include <FieldInfo.h>
#include <QSignalMapper>
#include <QTableWidget>
#include <QPushButton>
#include <QModelIndex>
#include <QMessageBox>
#include <TableController.h>

namespace Ui {
class AddAttributeWindow;
}

class AddAttributeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddAttributeWindow(QWidget *parent = 0,QTableWidget *tableWidget = 0);
    ~AddAttributeWindow();

    FieldInfo* getNewField();

public slots:
    void saveData();
    void addValue();
    void removeValue(int id);
    void populateWindow(const QString fieldType);
    void setDefaultMin(int min);
    void setDefaultMax(int max);

private:
    Ui::AddAttributeWindow *ui;
    FieldInfo newField;
    int newValueID;
    QTableWidget *tableWidget;
    QSignalMapper *signalMapper;
};

#endif // ADDATTRIBUTEWINDOW_H
