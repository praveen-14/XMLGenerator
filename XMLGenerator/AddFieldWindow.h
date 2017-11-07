#ifndef ADDFIELDWINDOW_H
#define ADDFIELDWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QScrollArea>
#include <TableController.h>
#include <QMessageBox>

namespace Ui {
class AddFieldWindow;
}
class AddFieldWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddFieldWindow(QWidget *parent = 0,QTableWidget *tableWidget = 0 );
    ~AddFieldWindow();

    QMap<QString,QString>* getWindowData();

public slots:
    void populateAttributes();
    void saveData();

private:
   Ui::AddFieldWindow *ui;
   QMap<QString,QString> windowData;
   QTableWidget *tableWidget;
};
#endif // ADDFIELDWINDOW_H
