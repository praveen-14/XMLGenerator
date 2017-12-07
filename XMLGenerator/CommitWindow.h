#ifndef COMMITWINDOW_H
#define COMMITWINDOW_H

#include <QDialog>
#include <MainWindow.h>

namespace Ui {
class CommitWindow;
}

class CommitWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CommitWindow(QWidget *parent = 0);
    ~CommitWindow();

public slots:
    void commit();

private:
    Ui::CommitWindow *ui;
};

#endif // COMMITWINDOW_H
