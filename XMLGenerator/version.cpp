#include "Version.h"

Version::Version(QString commitID)
{
    this->commitID = commitID;
    QList<QString> commits = commitID.split("/");
    this->versionID = commits.at(commits.size()-1);
}

Version::~Version()
{

}

void Version::setCommitMessage(QString commitMessage)
{
    this->commitMessage = commitMessage;
}

QString Version::getCommitMessage()
{
    return this->commitMessage;
}

QString Version::getCommitID()
{
    return commitID;
}

QString Version::getVersionID()
{
    return versionID;
}

int Version::size(Version *root, int currentSize)
{
    if(root){
        currentSize++;
        foreach (Version *subVersion, *(root->getSubVersions())) {
            currentSize = size(subVersion, currentSize);
        }
    }
    return currentSize;
}

void Version::addSubVersions(QList<Version*> versions)
{
    subVersions.append(versions);
}

Version* Version::findVersion(QString commitID)
{
    if(this->getCommitID().compare(commitID) == 0){
        return this;
    }
    foreach (Version *version, subVersions) {
//        if(version->getCommitID().compare(commitID) == 0){
//            return version;
//        }else{
        Version *foundVersion = version->findVersion(commitID);
        if(foundVersion){
            return foundVersion;
        }
//        }
    }
    return nullptr;
}

QList<Version*>* Version::getSubVersions()
{
    return &subVersions;
}
