/********************************************************
FileName: 	config.cpp
Author: 	czl
Email: 		137493336@qq.com
Created: 	2019/01/09
Description:配置文件类实现
********************************************************/

#include "config.h"

namespace iniFile_parse
{
	/************************************
	@ Brief:		ConfigItem构造函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	************************************/
	ConfigItem::ConfigItem()
	{
		m_key = new char[MAX_CONFIG_LINE_LENGTH];
		m_value = new char[MAX_CONFIG_LINE_LENGTH];
	}

	/************************************
	@ Brief:		ConfigItem带参构造函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		key:键值，value:值
	************************************/
	ConfigItem::ConfigItem(const char *key, const char *value)
	{
		m_key = new char[MAX_CONFIG_LINE_LENGTH];
		m_value = new char[MAX_CONFIG_LINE_LENGTH];
		strcpy(m_key, key);
		strcpy(m_value, value);
	}

	/************************************
	@ Brief:		ConfigItem拷贝构造函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		tmp:待拷贝的配置项
	************************************/
	ConfigItem::ConfigItem(const ConfigItem &tmp)
	{
		m_key = new char[MAX_CONFIG_LINE_LENGTH];
		m_value = new char[MAX_CONFIG_LINE_LENGTH];
		strcpy(m_key, tmp.m_key);
		strcpy(m_value, tmp.m_value);
	}

	/************************************
	@ Brief:		ConfigItem析构函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	************************************/
	ConfigItem::~ConfigItem()
	{
		delete[] m_key;
		delete[] m_value;
		m_key = NULL;
		m_value = NULL;
	}

	/************************************
	@ Brief:		清空配置项
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		NA
	************************************/
	void ConfigItem::clear()
	{
		delete[] m_key;
		delete[] m_value;
		m_key = NULL;
		m_value = NULL;
	}

	/************************************
	@ Brief:		赋值运算符重载
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		ConfigItem类型引用
	@ Return:		ConfigItem类型引用
	************************************/
	ConfigItem& ConfigItem::operator = (const ConfigItem& tmp)
	{
		strcpy(this->m_key, tmp.m_key);
		strcpy(this->m_value, tmp.m_value);
		return *this;
	}

	/************************************
	@ Brief:		设置key
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		待设置字符串
	@ Return:		NA
	************************************/
	void ConfigItem::set_key(const char *tmp)
	{
		strcpy(m_key, tmp);
	}

	/************************************
	@ Brief:		获取key
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		返回键值
	************************************/
	char *ConfigItem::get_key() const
	{
		return m_key;
	}

	/************************************
	@ Brief:		设置value
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		待设置字符串
	@ Return:		NA
	************************************/
	void ConfigItem::set_value(const char *tmp)
	{
		strcpy(m_value, tmp);
	}

	/************************************
	@ Brief:		获取value
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		返回value值
	************************************/
	char *ConfigItem::get_value() const
	{
		return m_value;
	}

	/************************************
	@ Brief:		将value的值以int类型返回
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		以int类型返回value值
	************************************/
	int ConfigItem::as_int() const
	{
		int value = atoi(m_value);
		if (value == 0)
		{
			printf("Warning | ConfigItem::as_int() is 0, may be failed.\n");
		}
		return value;
	}

	/************************************
	@ Brief:		将value的值以double类型返回
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		以double类型返回value值
	************************************/
	double ConfigItem::as_double() const
	{
		char **end_ptr = NULL;
		double value = strtod(m_value, end_ptr);
		if (value == 0)
		{
			printf("Warning |  ConfigItem::as_double() is 0, may be failed.\n");
		}
		return value;
	}

	/************************************
	@ Brief:		将value的值以string类型返回（直接用char *赋值string类型变量即可）
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		以string类型返回value值
	************************************/
	string ConfigItem::as_string() const
	{
		string value = m_value;
		return value;
	}

	/************************************
	@ Brief:		Config构造函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	************************************/
	Config::Config()
	{
		m_item_map.clear();
		p_item = new ConfigItem();
	}

	/************************************
	@ Brief:		拷贝构造函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	************************************/
	Config::Config(const Config &tmp)
	{
		m_item_map.clear();
		*p_item = *tmp.p_item;
		m_item_map = tmp.m_item_map;
	}

	/************************************
	@ Brief:		Config析构函数
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	************************************/
	Config::~Config()
	{
		m_item_map.clear();
		delete p_item;
	}

	/************************************
	@ Brief:		配置文件类，赋值运算符重载
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		静态配置文件类引用
	@ Return:		配置文件类引用
	************************************/
	Config& Config::operator = (const Config& tmp)
	{
		this->m_item_map.clear();
		*(this->p_item) = *tmp.p_item;
		this->m_item_map = tmp.m_item_map;
		return *this;
	}

	/************************************
	@ Brief:		清空配置表
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		NA
	************************************/
	void Config::clear()
	{
		m_item_map.clear();
	}

	/************************************
	@ Brief:		获取配置表的大小
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		NA
	@ Return:		配置表的大小
	************************************/
	int Config::size()const
	{
		int size = m_item_map.size();
		return size;
	}

	/************************************
	@ Brief:		获取某一个配置表中的元素
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		key:键值,ptr:指向配置项的指针
	@ Return:		配置表的大小
	************************************/
	int Config::get_item(string key, ConfigItem *ptr)
	{
		map<string, ConfigItem>::iterator iter;
		iter = m_item_map.find(key);
		if (iter == m_item_map.end())
		{
			printf("ERROR | Config::get_item can't find the item with the key = %s.\n", key.c_str());
			return EC_MISMATCH;
		}
		else
		{
			*ptr = iter->second;
			return EC_NONE;
		}
	}

	/************************************
	@ Brief:		设置并添加配置表中的元素
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		key:键值,value:指向配置项m_value值的指针
	@ Return:		NA
	************************************/
	void Config::set_item(string key, const char *value)
	{
		map<string, ConfigItem>::iterator iter;
		p_item->set_key(key.c_str());
		p_item->set_value(value);
		iter = m_item_map.find(key);
		//搜到键值，直接修改
		if (iter != m_item_map.end())
		{
			m_item_map[key] = *p_item;
		}
		else//搜不到键值，则添加
		{
			m_item_map.insert(make_pair(key, *p_item));
		}
	}

	/************************************
	@ Brief:		以数组下标的形式来获取配置项引用
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		key:键值
	@ Return:		配置项引用
	************************************/
	ConfigItem &Config::operator[](string key)
	{
		map<string, ConfigItem>::iterator iter;
		iter = m_item_map.find(key);
		if (iter == m_item_map.end())
		{
			printf("ERROR | const ConfigItem & Config::operator[] can't find the item with the key = %s.\n", key.c_str());
			p_item->m_key = NULL;
			p_item->m_value = NULL;
			return *p_item;
		}
		else
		{
			*p_item = iter->second;
			char *value = p_item->get_value();
			return *p_item;
		}
	}

	/************************************
	@ Brief:		解析配置文件
	@ Author:		czl
	@ Created: 		2019/01/09
	@ Param:		fileName:配置文件路径与名称,pconfig:配置指针
	@ Return:		解析状态
	************************************/
	int parse_ini_file(const char *fileName, Config *pconfig)
	{
		char buffer[MAX_CONFIG_LINE_LENGTH];
		pconfig->clear();

		FILE *fp = fopen(fileName, "rt");

		if (fp == NULL)
		{
			printf("ERROR | parse_ini_file open the file failed.\n");
			return EC_FORMAT;
		}
		while (fgets(buffer, MAX_CONFIG_LINE_LENGTH, fp) != NULL)
		{
			if (buffer[0] == '#')
				continue;
			char *key = strtok(buffer, "\n\r=");
			if (key == NULL)
			{
				continue;
			}
			const char *value = strtok(NULL, "\n\r");
			if (value == NULL)
			{
				value = "";
			}
			pconfig->set_item(key, value);
		}
		fclose(fp);

		return EC_NONE;
	}
}