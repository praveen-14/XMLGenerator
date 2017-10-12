#include "FieldInfo.h"

FieldInfo::FieldInfo(QObject *parent)
	: QObject(parent)
{
}

FieldInfo::~FieldInfo()
{
}

QString FieldInfo::name() const
{
	return m_name;
}

void FieldInfo::setName(const QString &name)
{
	m_name = name;
}

QString FieldInfo::displayName() const
{
	return m_displayName;
}

void FieldInfo::setDisplayName(const QString &displayName)
{
	m_displayName = displayName;
}

FieldType FieldInfo::fieldType() const
{
	return m_fieldType;
}

void FieldInfo::setFieldType(const FieldType &fieldType)
{
	m_fieldType = fieldType;
}

bool FieldInfo::isMandetory() const
{
	return m_isMandetory;
}

void FieldInfo::setIsMandetory(bool isMandetory)
{
	m_isMandetory = isMandetory;
}

QString FieldInfo::defaultVal() const
{
	return m_defaultVal;
}

void FieldInfo::setDefaultVal(const QString &defaultVal)
{
	m_defaultVal = defaultVal;
}

int FieldInfo::minRange() const
{
	return m_minRange;
}

void FieldInfo::setMinRange(int minRange)
{
	m_minRange = minRange;
}

int FieldInfo::maxRange() const
{
	return m_maxRange;
}

void FieldInfo::setMaxRange(int maxRange)
{
	m_maxRange = maxRange;
}

QMap<QString, QString>* FieldInfo::dropDownValMap()
{
	return &m_dropDownValMap;
}
