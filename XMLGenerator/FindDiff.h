#ifndef FINDDIFF_H
#define FINDDIFF_H

#include <QDialog>
#include <MainWindow.h>
#include <QCheckBox>
#include <Change.h>
#include <TableController.h>

namespace Ui {
class FindDiff;
}

class FindDiff : public QDialog
{
    Q_OBJECT

public:
    explicit FindDiff(QWidget *parent = 0,XMLFileData *mergedFile = 0, QList<Change*> *newChanges = 0, CacheConfig *config = 0);
    ~FindDiff();

    void populateWindow();

public slots:
//    void selectionChanged();
    void saveMerge();
//    void tableItemClicked(int row, int column);

private:
    Ui::FindDiff *ui;
    XMLFileData *mergedFile;
    QList<Change*> *newChanges;
    CacheConfig *config;
    QMap<Change*,QTableWidgetItem*> changesMap;
};

#endif // FINDDIFF_H
