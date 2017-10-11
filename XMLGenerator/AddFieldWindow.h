#ifndef ADDFIELDWINDOW_H
#define ADDFIELDWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <TableController.h>

namespace Ui {
class AddFieldWindow;
}
class AddFieldWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddFieldWindow(QWidget *parent = 0);
    ~AddFieldWindow();
    QMap<QString,QString>* getWindowData();

public slots:
    void populateAttributes(Table* table);
    void saveData();

private:
   Ui::AddFieldWindow *ui;
   QMap<QString,QString> windowData;
};
#endif // ADDFIELDWINDOW_H
