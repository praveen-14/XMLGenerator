#ifndef ADDENUM_H
#define ADDENUM_H

#include <QDialog>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDebug>
#include <Enum.h>
#include <MainWindow.h>

class TableController;

namespace Ui {
class AddEnum;
}

class AddEnum : public QDialog
{
    Q_OBJECT

public:
    explicit AddEnum(QWidget *parent = 0, QString *filePath = 0);
    ~AddEnum();

    void setEnumName(QString enumName);
    void makeNecessary();
    void makeUnclosable();

public slots:
    void addValue();
    void removeValue(int id);
    void saveEnum();

private:
    Ui::AddEnum *ui;
    QSignalMapper *signalMapper;
    int newValueID;
    QString *filePath;
};

#endif // ADDENUM_H
