#ifndef ADDATTRIBUTEWINDOW_H
#define ADDATTRIBUTEWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QCheckBox>

namespace Ui {
class AddAttributeWindow;
}

class AddAttributeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddAttributeWindow(QWidget *parent = 0);
    ~AddAttributeWindow();
    void initializeWindow();
    QMap<QString,QString>* getWindowData();

public slots:
    void saveData();

private:
    Ui::AddAttributeWindow *ui;
    QMap<QString,QString> windowData;
};

#endif // ADDATTRIBUTEWINDOW_H
