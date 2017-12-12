#include "CommitWindow.h"
#include "ui_CommitWindow.h"

CommitWindow::CommitWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommitWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(this->width(), this->height()));
}

CommitWindow::~CommitWindow()
{
    delete ui;
}

void CommitWindow::commit()
{
    if(ui->commitMessageLineEdit->text().trimmed().size() > 0)
    {
        MainWindow::mainWindow->setCommitMessage(ui->mergeLabel->text() + ui->commitMessageLineEdit->text());
        accept();
    }
    else
    {
        QMessageBox msg(QMessageBox::Warning, "Fail to commit", "Commit message cannot be empty.");
        msg.exec();
    }
}

void CommitWindow::mergeCommit()
{
    ui->mergeLabel->setText("product_merge_");
}

void CommitWindow::makeNecessary()
{
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
}
