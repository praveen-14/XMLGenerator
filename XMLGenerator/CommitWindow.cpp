#include "CommitWindow.h"
#include "ui_CommitWindow.h"

CommitWindow::CommitWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommitWindow)
{
    ui->setupUi(this);
}

CommitWindow::~CommitWindow()
{
    delete ui;
}

void CommitWindow::commit()
{
    MainWindow::mainWindow->setCommitMessage(ui->commitMessageLineEdit->text());
    accept();
}
