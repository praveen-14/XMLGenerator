#pragma once

#include <QObject>
#include <QMap>

enum FieldType
{
	Invalid = -1,
	Text = 0,
	DropDown,
	Bool,
	Integer
};

class FieldInfo : public QObject
{
	Q_OBJECT

public:
	FieldInfo(QObject *parent);
	~FieldInfo();

	QString name() const;
	void setName(const QString &name);

	QString displayName() const;
	void setDisplayName(const QString &displayName);

	FieldType fieldType() const;
	void setFieldType(const FieldType &fieldType);

	bool isMandetory() const;
	void setIsMandetory(bool isMandetory);

	QString defaultVal() const;
	void setDefaultVal(const QString &defaultVal);

	int minRange() const;
	void setMinRange(int minRange);

	int maxRange() const;
	void setMaxRange(int maxRange);

	QMap<QString, QString>* dropDownValMap();

private:
	QString m_name;
	QString m_displayName;
	FieldType m_fieldType;
	bool m_isMandetory;
	QString m_defaultVal;
	int m_minRange;
	int m_maxRange;
	QMap<QString, QString> m_dropDownValMap;
};
