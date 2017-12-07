#pragma once
#include <QObject>
#include "qxmlstream.h"
#include "FieldInfo.h"
#include <QDomDocument>

class CacheConfig : QObject
{
	Q_OBJECT

public:
    CacheConfig(QObject *parent);
	virtual ~CacheConfig();
    bool init(QString filePath);

	QList<FieldInfo *>* cacheFieldList();
	QList<FieldInfo *>* messageFieldList();
	QList<FieldInfo *>* columnFieldList();

	int posForColumnName(QString name);
	int posForMessageName(QString name);
	int posForCacheName(QString name);

    void addColumnField(FieldInfo *field);

private:
	void populateFieldInfo(QXmlStreamReader* reader, QList<FieldInfo *>* container);
	void loadEnumsFromFile(QString file, FieldInfo* info);

	QList<FieldInfo *> m_cacheFieldsList;
	QList<FieldInfo *> m_messageFieldsList;
	QList<FieldInfo *> m_columnFieldsList;
};

