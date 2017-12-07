#ifndef VERSION_H
#define VERSION_H

#include <QObject>
#include <QList>

class Version
{
public:
    Version(QString commitID);
    ~Version();

    void setCommitMessage(QString commitMessage);
    QString getCommitMessage();

    QString getCommitID();

    QString getVersionID();

    int size(Version *root, int currentSize = 0);

    void addSubVersions(QList<Version*> versions);
    Version* findVersion(QString commitID);
    QList<Version*>* getSubVersions();

private:
    QString versionID;
    QString commitID;
    QString commitMessage;
    QList<Version*> subVersions;
};

#endif // VERSION_H
