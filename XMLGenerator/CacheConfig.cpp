#include "CacheConfig.h"
#include <QFile>
#include <QDebug>
#include <QMessagebox>

#define XML_FIELD_PROPERTY		"field"
#define XML_NAME_PROPERTY		"name"
#define XML_DISP_NAME_PROPERTY	"displayName"
#define XML_FIELD_TYPE_PROPERTY "fieldType"
#define XML_MANDETORY_PROPERTY	"isMandetory"
#define XML_NULLABLE_PROPERTY	"nullable"
#define XML_DEFAULT_PROPERTY	"default"
#define XML_ITEM_PROPERTY		"item"
#define XML_VALUE_PROPERTY		"value"
#define XML_MIN_RANGE_PROPERTY	"minRange"
#define XML_MAX_RANGE_PROPERTY	"maxRange"
#define XML_ENUM_FILE_PROPERTY	"enumFile"

CacheConfig::CacheConfig(QObject *parent)
	: QObject(parent)
{
}


CacheConfig::~CacheConfig()
{
}

void CacheConfig::addColumnField(FieldInfo *field)
{
    m_columnFieldsList.append(field);
//    QDomDocument doc("tempCacheConfig.xml");
//    QFile file("F:/Projects/My Projects/build-XMLGenerator-Desktop_Qt_5_9_0_MinGW_32bit-Debug/CacheXMLConfigs.xml");
//    if (!file.open(QIODevice::ReadOnly)) {
//        qDebug() << "Cannot open the file";
//        return;
//    }
//    // Parse file
//    if (!doc.setContent(&file)) {
//       qDebug() << "Cannot parse the content";
//       file.close();
//       return;
//    }
//    file.close();

//    // Modify content
//    QDomNodeList columnFieldsArray = elementsByTagName("column_fields");
//    if (roots.size() < 1) {
//       qDebug() << "Cannot find root";
//       return;
//    }
//    QDomNode columnFieldNode = columnFieldsArray.at(0);
//    QDomNode newNode = new QDomNode();
//    QDomElement newElement = newNode.toElement();
//    newElement.setAttribute("name",field->name());
//    newElement.setAttribute("displayName",field->displayName());
//    newElement.setAttribute("fieldType",FieldType(field->fieldType()));
//    newElement.setAttribute("default",field->defaultVal());
//    if(field->isMandetory()){
//        newElement.setAttribute("isMandetory","true");
//    }else
//        newElement.setAttribute("isMandetory","false");
//    if(field->nullable()){
//        newElement.setAttribute("nullable","true");
//    }else
//        newElement.setAttribute("nullable","false");
//    if(field->fieldType() == FieldType::Integer){
//        newElement.setAttribute("minRan","false");
//        newElement.setAttribute("nullable","false");
//    }
//    // Then do the same thing for somechild
//    ...

//    // Save content back to the file
//    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
//        qDebug() << "Basically, now we lost content of a file";
//        return;
//    }
//    QByteArray xml = doc.toByteArray();
//    file.write(xml);
//    file.close();
}

bool CacheConfig::init(QString filePath)
{
    QFile inputFile(filePath);
    if (inputFile.open(QIODevice::ReadOnly))
        {
            QXmlStreamReader reader(&inputFile);

            while (!reader.atEnd())
            {
                reader.readNext();

                if (reader.name().compare(QString("cache_fields"), Qt::CaseInsensitive) == 0)
                {
                    qDebug() << "****************cache****************";
                    while (reader.readNextStartElement() && reader.name().compare(QString(XML_FIELD_PROPERTY), Qt::CaseInsensitive) == 0)
                    {
                        populateFieldInfo(&reader, &m_cacheFieldsList);
                    }
                }
                else if (reader.name().compare(QString("message_fields"), Qt::CaseInsensitive) == 0)
                {
                    qDebug() << "****************message****************";
                    while (reader.readNextStartElement())
                    {
                        populateFieldInfo(&reader, &m_messageFieldsList);
                    }
                }
                else if (reader.name().compare(QString("column_fields"), Qt::CaseInsensitive) == 0)
                {
                    qDebug() << "****************column****************";
                    while (reader.readNextStartElement())
                    {
                        populateFieldInfo(&reader, &m_columnFieldsList);
                    }
                }

            }
            inputFile.close();
        }
        else
        {
            return false;
        }
        return true;
}

QList<FieldInfo*>* CacheConfig::cacheFieldList()
{
	return &m_cacheFieldsList;
}

QList<FieldInfo*>* CacheConfig::messageFieldList()
{
	return &m_messageFieldsList;
}

QList<FieldInfo*>* CacheConfig::columnFieldList()
{
	return &m_columnFieldsList;
}

int CacheConfig::posForColumnName(QString name)
{
	int i = 0;
	bool found = false;

	for(; i < m_columnFieldsList.size();++i)
	{
		if(m_columnFieldsList.at(i)->name() == name)
		{
			found = true;
			break;
		}
	}

	if (found)
		return i;

	return -1;
}

int CacheConfig::posForMessageName(QString name)
{
	int i = 0;
	bool found = false;

	for (; i < m_messageFieldsList.size(); ++i)
	{
		if (m_messageFieldsList.at(i)->name() == name)
		{
			found = true;
			break;
		}
	}

	if (found)
		return i;

	return -1;
}

int CacheConfig::posForCacheName(QString name)
{
	int i = 0;
	bool found = false;

	for (; i < m_cacheFieldsList.size(); ++i)
	{
		if (m_cacheFieldsList.at(i)->name() == name)
		{
			found = true;
			break;
		}
	}

	if (found)
		return i;

	return -1;
}

void CacheConfig::populateFieldInfo(QXmlStreamReader* reader, QList<FieldInfo*>* container)
{
	QXmlStreamAttributes attrs = reader->attributes();

	if (container)
	{
		FieldInfo* info = new FieldInfo(this);
		info->setName(attrs.value(XML_NAME_PROPERTY).toString());
		info->setDisplayName(attrs.value(XML_DISP_NAME_PROPERTY).toString());
		info->setFieldType(FieldType(attrs.value(XML_FIELD_TYPE_PROPERTY).toInt()));
		info->setDefaultVal(attrs.value(XML_DEFAULT_PROPERTY).toString());
		info->setIsMandetory(attrs.value(XML_MANDETORY_PROPERTY).toString() == "true" ? true : false);
        info->setNullable(attrs.value(XML_NULLABLE_PROPERTY).toString() == "true" ? true : false);

		qDebug() << attrs.value(XML_NAME_PROPERTY).toString();

		if (info->fieldType() == FieldType::Integer)
		{
			if (!attrs.value(XML_MIN_RANGE_PROPERTY).isNull())
				info->setMinRange(attrs.value(XML_MIN_RANGE_PROPERTY).toInt());
			if (!attrs.value(XML_MIN_RANGE_PROPERTY).isNull())
				info->setMaxRange(attrs.value(XML_MAX_RANGE_PROPERTY).toInt());
		}
		else if (info->fieldType() == FieldType::DropDown)
		{
			qDebug() << "Start ***** <DropDown>";

			QString enumFile = attrs.value(XML_ENUM_FILE_PROPERTY).toString();
			if(!enumFile.isEmpty())
			{ 
				loadEnumsFromFile(enumFile, info);
                reader->skipCurrentElement();
			}
			else
			{
                while (reader->readNextStartElement() && reader->name().compare(QString(XML_ITEM_PROPERTY), Qt::CaseInsensitive) == 0)
				{
					attrs = reader->attributes();
					qDebug() << attrs.value(XML_NAME_PROPERTY).toString() << " - " << attrs.value(XML_VALUE_PROPERTY).toString();
					info->dropDownValMap()->insert(attrs.value(XML_NAME_PROPERTY).toString(), attrs.value(XML_VALUE_PROPERTY).toString());
					reader->skipCurrentElement();
				}
			}
			//reader->readNextStartElement();
			qDebug() << "End ***** <DropDown>";
		}
		container->push_back(info);

		if (info->fieldType() != FieldType::DropDown)
			reader->skipCurrentElement();
	}
}

void CacheConfig::loadEnumsFromFile(QString file, FieldInfo* info)
{
	QFile outputFile(file);
	if (outputFile.open(QIODevice::ReadOnly))
	{
		QXmlStreamReader reader(&outputFile);

		bool enumFound = false;

		while (!reader.atEnd())
		{
			reader.readNext();

			QString elementName = reader.name().toString();
			if (reader.isStartElement())
			{
				QXmlStreamAttributes attrs = reader.attributes();

				if (elementName.compare("Enum", Qt::CaseInsensitive) == 0)
				{
					QStringRef name = attrs.value("name");
					if (!name.isNull())
					{
						enumFound = true;
						info->dropDownValMap()->insert(name.toString(), name.toString());
					}
				}
			}

		}
		outputFile.close();

		if (!enumFound)
		{
			QMessageBox msg(QMessageBox::Warning, "Fail to open File", "File is not in the correct format to load Enums");
			msg.exec();
		}
		else
		{
            qDebug() << "enum loaded successfully";
		}
	}
	else
	{
		QMessageBox msg(QMessageBox::Warning, "Fail to open File", outputFile.errorString());
		msg.exec();
	}
}
