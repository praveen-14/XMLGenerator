// Copyright(c) 2015; Owned by Millennium IT Software (Private) Limited; a member of the London Stock Exchange Group.
// All rights reserved; Do not copy, reproduce, disclose or transmit without the express written approval of Owner.
#pragma once

#include <qnamespace.h>
#include <QColor>
#include <QtXml>
#include <AppCommonDefs.h>

#define CACHE_TAG_XML					"cache"
#define CACHE_NAME_TAG_XML				"name"
#define CACHE_TABLE_NAME_TAG_XML		"tableName"
#define CACHE_MESSAGE_HISTORY_KEY		"historyKey"
#define CACHE_MESSAGE_KEY				"key"
#define CACHE_MESSAGE_TIME				"time"
#define CACHE_MESSAGE_TAG_XML			"message"
#define CACHE_COLUMN_TAG_XML			"Column"
#define CACHE_DEFAULT_COLOR_TAG			"DefaultColors"
#define CACHE_DEFAULT_COLOR_CONDITION	"Condition"

#define CACHE_MESSAGE_TAB_ID			"tabId"

#define COLUMN_FIELD_NAME_TAG			"name"
#define COLUMN_DISPLAY_NAME_TAG			"displayName"
#define COLUMN_WIDTH_TAG				"width"
#define COLUMN_ALIGN_TAG				"align"
#define COLUMN_DECIMAL_PLACES			"Decimals"
#define COLUMN_SHOW_ZERO				"showZeros"
#define COLUMN_BACKGROUND				"background"
#define COLUMN_COMPOSITE				"composite"
#define COLUMN_SHOW						"show"
#define COLUMN_DB_COL_NAME				"dbColumn"
#define COLUMN_DT_FORMAT_DISPALY		"dtDispFmt"
#define COLUMN_ENUM_NAME				"enumName"

#define CONDITION_COLOR					"color"
#define CONDITION_COLUMN				"column"
#define CONDITION_OPERATOR				"ope"
#define CONDITION_FIELD_VALUE			"fieldValue"
#define CONDITION_TYPE					"type"
#define CONDITION_COLUMN_LIST			"colorColumns"
#define CONDITION_PRIORITY				"priority"

class ColumnProperty
{
public:
	ColumnProperty()
	{
		m_width = 100;
		m_index = -1;
		m_decimalPlaces = 2;
		m_dtFormat = -1;

		m_showZero = false;
		m_isComposite = false;
		m_show = false;

		m_fieldName = "";
		m_displayName = "";

		m_alignment = Qt::AlignCenter;
		m_background = Qt::transparent;
	}


	bool getIsComposite(){return m_isComposite;}
	int getIndex(){return m_index;}

	QString getFieldName(){return m_fieldName;}
	void setFiledName(QString name){m_fieldName = name;}
	
	QString getDisplayName(){return m_displayName.isEmpty() ?m_fieldName : m_displayName;}
	void setDisplayName(QString name){m_displayName = name;}

	Qt::AlignmentFlag getAlignment(){return m_alignment;}
	void setAlignment(Qt::AlignmentFlag alignment){m_alignment = alignment;}
	
	int getWidth(){return m_width;}
	void setWidth(int width){m_width = width;}
	
	bool getShowZero(){return m_showZero;}
	void setShowZero(bool show){m_showZero = show;}

	bool getIsShow() { return m_show; }
	void setShow(bool show) { m_show = show; }
	
	int getDecimalPlaces(){return m_decimalPlaces;}
	void setDecimalPlaces(int decimal){m_decimalPlaces = decimal;}
	
	QColor getBackgroundColor(){return m_background;}
	void setbackground(QColor color){m_background = color;}
	
	void setComposite(bool composite){m_isComposite = composite;}
	void setIndex(int index){m_index = index;}

	void setDBCol(QString dbcol) { m_dbColName = dbcol; }
	QString getDBCol() const { return m_dbColName.isEmpty() ? m_fieldName : m_dbColName; }

	int getDTFormat() { return m_dtFormat;  }
	void setDTFormat(int dtFormat) { m_dtFormat = dtFormat;  }

	void setEnumName(QString enumName) { m_enumName = enumName; }
	QString getEnumName() const { return m_enumName; }
	 
private:
	int m_width;
	int m_decimalPlaces;
	int m_index;
	int m_dtFormat;

	bool m_showZero;
	bool m_isComposite;
	bool m_show;

	QString m_fieldName;
	QString m_displayName;
	QString m_dbColName;
	QString m_enumName;

	Qt::AlignmentFlag m_alignment; 
	
	QColor m_background;
};

class ColorConditions
{
public:
	enum ColorConditionsOperators
	{
		CC_EQUAL = 0,
		CC_NOT_EQUAL,
		CC_LESS_THAN,
		CC_GREATER_THAN,
		CC_LESS_THAN_OR_EQUAL,
		CC_GREATER_THAN_OR_EQUAL
	};
	enum ColorConditionsType
	{
		CC_VALUE = 0,
		CC_COLUMN
	};
	ColorConditions(QString color,QString col,QString field,QString ope,QString type,QString priority,QString columns):
		m_color(color)
		,m_column(col)
		,m_ope(CC_NOT_EQUAL)
		,m_fieldValue(field)
		,m_type(CC_COLUMN)
		,m_priority(priority.toInt())
	{
		if (!columns.isEmpty())
		{
			m_columns = columns.split("|");
		}
		if(!type.isEmpty() && type.compare("value",Qt::CaseInsensitive) == 0)
		{
			m_type = CC_VALUE;
		}
		if (ope.compare("!=") == 0)
		{
			m_ope = CC_NOT_EQUAL;
		}
		else if (ope.compare("==") == 0)
		{
			m_ope = CC_EQUAL;
		}
		else if (ope.compare("<") == 0)
		{
			m_ope = CC_LESS_THAN;
		}
		else if (ope.compare(">") == 0)
		{
			m_ope = CC_GREATER_THAN;
		}
		else if (ope.compare("<=") == 0)
		{
			m_ope = CC_LESS_THAN_OR_EQUAL;
		}
		else if (ope.compare(">=") == 0)
		{
			m_ope = CC_GREATER_THAN_OR_EQUAL;
		}


	}

	virtual ~ColorConditions()
	{
		
	}

	QString getColor() const { return m_color; }
	QString getColumn() const { return m_column; }
	ColorConditionsOperators getOperator() const { return m_ope; }
	QString getFieldValue() const { return m_fieldValue; }
	ColorConditionsType getType() const { return m_type; }
	int getPriority() const { return m_priority; }
	bool isValidForColoring(QString colName,int priority = 0)
	{
		return (
			m_columns.empty() ||
			std::find(m_columns.begin(), m_columns.end(), colName) != m_columns.end()
			) &&
			m_priority >= priority;
	}

	template<typename T>
	bool checkCondition(T left,T right)
	{
		switch(m_ope)
		{
		case CC_EQUAL: return left == right;
		case CC_NOT_EQUAL: return left != right;
		case CC_LESS_THAN: return left < right;
		case CC_GREATER_THAN: return left > right;
		case CC_LESS_THAN_OR_EQUAL: return left <= right;
		case CC_GREATER_THAN_OR_EQUAL: return left >= right;
		default: break;
		}
		return false;
	}


private:
	QString m_color;
	QString m_column;
	ColorConditionsOperators m_ope;
	QString m_fieldValue;
	ColorConditionsType m_type;
	QStringList m_columns;
	int m_priority;


};



class CacheConfig
{
public:
	CacheConfig()
	{
		m_cacheName = "";
		m_tableName = "";
		m_historyKey = "";
		m_key = "";
		m_tabID = "";
		m_time = "";
		m_baseTemplateName = "";
	}

	virtual ~CacheConfig()
	{
		for (auto itr = m_columnConfigs.begin(); itr != m_columnConfigs.end();++itr)
		{
			delete (*itr);
		}
		for (auto itr = m_conditions.begin(); itr != m_conditions.end(); ++itr)
		{
			delete (*itr);
		}
	}
	
	QList<ColumnProperty*>* getColumns(){return &m_columnConfigs;}
	void addColumn(int index, QString colName, QString displayName, Qt::AlignmentFlag alingment = Qt::AlignCenter 
				, int colWidth = 100, bool showZero = true, int decimals = 4, QColor backColor = Qt::transparent, 
		bool isComposite = false, bool isShown = true,QString dbCol = "", int dtFormat = -1, QString enumName = "")
	{
		ColumnProperty* column = new ColumnProperty();
		
		index = (index<0) ? m_columnConfigs.size() : index;
		column->setIndex(index);
		column->setFiledName(colName);
		column->setDisplayName(displayName);
		column->setAlignment(alingment);
		column->setWidth(colWidth);
		column->setShowZero(showZero);
		column->setDecimalPlaces(decimals);
		column->setbackground(backColor);
		column->setComposite(isComposite);
		column->setShow(isShown);
		column->setDBCol(dbCol);
		column->setDTFormat(dtFormat);
		column->setEnumName(enumName);

		m_columnConfigs.push_back(column);
		m_colNames[colName] = isShown;
		m_colDecimals[colName] = decimals;
	}

	QString getCacheName(){return m_cacheName;}
	void setCacheName(QString name){m_cacheName = name;}

	QString getTableName(){return m_tableName;}
	void setTableName(QString name){m_tableName = name;}

	QString getHistoryKey(){return m_historyKey;}
	void setHistoryKey(QString key){m_historyKey = key;}

	QString getKey(){return m_key;}
	void setKey(QString key){m_key = key;}

	QString getTabID() { return m_tabID; }
	void setTabID(QString tabID) { m_tabID = tabID; }

	void setTime(QString time) { m_time = time; }
	QString getTime() const { return m_time; }

	bool hasColumn(QString name)
	{
		auto itr = m_colNames.find(name);
		return itr != m_colNames.end();
	}
	bool isColumnVisible(QString name)
	{
		auto itr = m_colNames.find(name);
		return itr != m_colNames.end() && itr.value();
	}
	int getDecimalPlaces(QString name)
	{
		auto itr = m_colDecimals.find(name);
		return itr == m_colDecimals.end() ? 4 : itr.value();
	}

	void getSimilarColumnName(QString name,QList<QString>& out, Qt::CaseSensitivity cs = Qt::CaseInsensitive)
	{		
		for (auto itr = m_colNames.begin(); itr != m_colNames.end();++itr)
		{
			if(itr.key().compare(name,cs) == 0)
			{
				out.push_back(itr.key());
			}
		}
	}

	QString getDBColumnByColName(QString colName,bool chopAlias = true)
	{
		for (auto itr = m_columnConfigs.begin(); itr != m_columnConfigs.end(); ++itr )
		{
			if ((*itr)->getFieldName().compare(colName) == 0)
			{
				QString name = (*itr)->getDBCol();
				if (!chopAlias)
				{
					return name;
				}
				QStringList lst = name.split(" as ");
				return lst.at(0);
			}
		}
		return colName;
	}
	void addCondition(ColorConditions* cond) { m_conditions.push_back(cond); }
	QList<ColorConditions*>* getDefaultColorConditions() { return &m_conditions; }

	const QString getBaseTemplateName() const {	return m_baseTemplateName;	}
	void setBaseTemplateName(QString name) { m_baseTemplateName = name; }
private:
	QList<ColumnProperty*> m_columnConfigs;

	QMap<QString, bool> m_colNames;
	QMap<QString, int> m_colDecimals;

	QString m_cacheName;
	QString m_tableName;
	QString m_historyKey;
	QString m_key;
	QString m_tabID;
	QString m_time;
	QString m_baseTemplateName;

	QList<ColorConditions*> m_conditions;

};


class CacheConfigBuilder : public QXmlDefaultHandler
{
public:
	CacheConfig* generateConfig(QString fileName)
	{
		QXmlSimpleReader xmlSimpleReader;
		xmlSimpleReader.setContentHandler(this);
		
		if (m_config != nullptr)
		{
			delete m_config;
			m_config = nullptr;
		}

		m_config = new CacheConfig();

		QFile file(fileName);
		QXmlInputSource xmlInputSource(&file);
		xmlSimpleReader.parse(&xmlInputSource);

		CacheConfig* config = m_config;
		m_config = nullptr;

		return config;
	};

	bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
	{
		
		if (localName.compare(CACHE_COLUMN_TAG_XML)==0)
		{
			QString zero = atts.value(COLUMN_SHOW_ZERO);
			QString decimal = atts.value(COLUMN_DECIMAL_PLACES);
			QString show = atts.value(COLUMN_SHOW);
			QString colWidth = atts.value(COLUMN_WIDTH_TAG);
			QString align = atts.value(COLUMN_ALIGN_TAG);
			QString dtFromatatt = atts.value(COLUMN_DT_FORMAT_DISPALY);
			QString enumName = atts.value(COLUMN_ENUM_NAME);

			bool showZero = zero.isEmpty() || (zero.compare("true", Qt::CaseInsensitive) == 0);
			bool showCol = (show.isEmpty() || show.compare("true", Qt::CaseInsensitive) == 0);
			int decimalPlaces = decimal.isEmpty() ? 2 : decimal.toInt();
			int iColWidth = colWidth.isEmpty() ? 100 : colWidth.toInt();
			int alignment = align.isEmpty() ? Qt::AlignHCenter : align.toInt();
			int dtFromat = dtFromatatt.isEmpty() ? -1 : dtFromatatt.toInt();

			decimalPlaces = decimalPlaces < 0 ? 2 : decimalPlaces;

			m_config->addColumn(
				m_config->getColumns()->size(),
				atts.value(COLUMN_FIELD_NAME_TAG),
				atts.value(COLUMN_DISPLAY_NAME_TAG),
				static_cast<Qt::AlignmentFlag>(alignment),
				iColWidth,
				showZero,
				decimalPlaces,
				Qt::transparent,
				false,
				showCol,
				atts.value(COLUMN_DB_COL_NAME),
				dtFromat,
				enumName);

		}
		else if (localName.compare(CACHE_TAG_XML) ==0 )
		{
			m_config->setCacheName(atts.value(CACHE_NAME_TAG_XML));
			m_config->setTableName(atts.value(CACHE_TABLE_NAME_TAG_XML));		
			m_config->setTabID(atts.value(CACHE_MESSAGE_TAB_ID));
			m_config->setBaseTemplateName(atts.value(CACHE_XML_COMMON_ATTRIBUTE_BASE_TEMPLATE));
		}
		else if (localName.compare(CACHE_MESSAGE_TAG_XML) == 0 )
		{
			m_config->setHistoryKey(atts.value(CACHE_MESSAGE_HISTORY_KEY));
			m_config->setKey(atts.value(CACHE_MESSAGE_KEY));
			m_config->setTime(atts.value(CACHE_MESSAGE_TIME));
		}
		else if(localName.compare(CACHE_DEFAULT_COLOR_CONDITION) == 0 )
		{
			ColorConditions* cond = new ColorConditions(
				atts.value(CONDITION_COLOR),
				atts.value(CONDITION_COLUMN),
				atts.value(CONDITION_FIELD_VALUE),
				atts.value(CONDITION_OPERATOR),
				atts.value(CONDITION_TYPE),
				atts.value(CONDITION_PRIORITY),
				atts.value(CONDITION_COLUMN_LIST)
				);
			m_config->addCondition(cond);
		}
		return true;
	}

	static CacheConfigBuilder* getInstance(){return &s_cacheConfigbuilder;}

private:
	CacheConfig* m_config;
	static CacheConfigBuilder s_cacheConfigbuilder;

	CacheConfigBuilder():QXmlDefaultHandler(),m_config(nullptr){}
};

//CacheConfigBuilder CacheConfigBuilder::m_builder;