#ifndef ENUM_H
#define ENUM_H

#include <QObject>
#include <QMap>
#include <QDebug>

class Enum
{
public:
    Enum(QString name);

    QString getName();
    QMap<QString,QString>* getValues();

    void addValue(QString value, QString name);

private:
    QString name;
    QMap<QString,QString> values;
};

#endif // ENUM_H
