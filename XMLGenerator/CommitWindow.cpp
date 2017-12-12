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
    MainWindow::mainWindow->setCommitMessage(ui->mergeLabel->text() + ui->commitMessageLineEdit->text());
    accept();
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
