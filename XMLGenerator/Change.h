#ifndef CHANGE_H
#define CHANGE_H

#include <QAbstractItemModel>

enum ChangeType
{
    NotHandled = -1,
    UpdateValue, //0
    AddingField, //1
    DeleteField, //2
    AddingAttribute, //3
    DeleteAttribute, //4
    AddingEnum, //5
    ChangeFieldOrder //6
};

class Change : public QObject
{
    Q_OBJECT

public:
    explicit Change(QObject *parent = nullptr, ChangeType type = ChangeType::NotHandled, QString fieldName = "", QString attributeName = "",QString value = "");
    ~Change();

    ChangeType getType();
    QString getFieldName();
    QString getAttributeName();
    QString getValue();
    QString getOldValue();
    int getSolutionRow();
    int getSolutionColumn();
    QString getEnumName();

    void setSolutionRow(int row);
    void setSolutionColumn(int column);
    void setValue(QString value);
    void setOldValue(QString oldValue);
    void setEnumName(QString enumName);

    void addUpdate(QString key, QString value);
    QMap<QString,QString>* getUpdateData();

private:
    ChangeType type;
    QString fieldName;
    QString attributeName;
    QString enumName;
    QString value;
    QString oldValue;
    int solutionRow;
    int solutionColumn;
    QMap<QString,QString> updatedData;
};

#endif // CHANGE_H
