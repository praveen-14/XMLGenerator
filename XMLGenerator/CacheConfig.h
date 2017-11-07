#pragma once
#include <QObject>
#include "qxmlstream.h"
#include "FieldInfo.h"
#include <QDomDocument>

class CacheConfig : QObject
{
	Q_OBJECT

public:
	virtual ~CacheConfig();
	static CacheConfig* getInstance() { return &m_instance; }
	bool init();

	QList<FieldInfo *>* cacheFieldList();
	QList<FieldInfo *>* messageFieldList();
	QList<FieldInfo *>* columnFieldList();

	int posForColumnName(QString name);
	int posForMessageName(QString name);
	int posForCacheName(QString name);

    void addColumnField(FieldInfo *field);

private:
	CacheConfig(QObject *parent);
	void populateFieldInfo(QXmlStreamReader* reader, QList<FieldInfo *>* container);
	void loadEnumsFromFile(QString file, FieldInfo* info);

	static CacheConfig m_instance;


	QList<FieldInfo *> m_cacheFieldsList;
	QList<FieldInfo *> m_messageFieldsList;
	QList<FieldInfo *> m_columnFieldsList;
};

