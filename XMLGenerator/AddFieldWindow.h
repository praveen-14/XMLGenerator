#ifndef ADDFIELDWINDOW_H
#define ADDFIELDWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
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
    explicit AddFieldWindow(QWidget *parent = 0,QList<QString> *attributesList = 0,CacheConfig *config = 0);
    ~AddFieldWindow();
    QMap<QString,QString>* getWindowData();

public slots:
    void populateAttributes();
    void saveData();

private:
   Ui::AddFieldWindow *ui;
   QMap<QString,QString> windowData;
   QList<QString> attributesList;
   CacheConfig config;
};
#endif // ADDFIELDWINDOW_H
