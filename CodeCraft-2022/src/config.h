/********************************************************
FileName: 	config.h
Author: 	czl
Email: 		137493336@qq.com
Created: 	2019/01/09
Description:配置文件类定义
********************************************************/
#pragma  once
using namespace std;
#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include "cstdio"
#include "cstdlib"
#include "cstring"
#include "map"

#ifndef config_define
#define config_define

#define NULL 0	//基本类型定义

//Error Code 定义
#define EC_NONE				0		//没有错误
#define EC_WOULDBLOCK		-1		//将阻塞
#define EC_ERROR			-2		//其他报错
#define EC_ACCESS			-3		//可达错误
#define EC_OUT_OF_BOUNDS	-4		//超限错误
#define EC_OPERATION		-5		//运算错误
#define EC_INSUF_BUFFER		-6		//缓存不足
#define EC_READ				-7		//读错误
#define EC_WRITE			-8		//写错误
#define	EC_FORMAT			-9		//格式错误
#define EC_TIMEOUT			-10		//超时错误
#define EC_DISCARD			-11		//丢弃错误
#define EC_EXIST			-12		//已经存在
#define EC_PARAM			-13		//参数错误
#define EC_MISMATCH			-14		//不匹配错误
#define EC_NOENT			-15		//没有入口
#define EC_INVAL			-16		//无效

#define MAX_CONFIG_LINE_LENGTH 128	//单行配置最大长度

namespace iniFile_parse
{

	/************************************
	@ ClassName		ConfigItem
	@ Brief:		自定义类型，用于表示配置文件中的配置项
	@ Author:		czl
	@ Created: 		2019/01/09
	************************************/
	class ConfigItem
	{
	public:
		ConfigItem();
		ConfigItem(const char *key, const char *value);
		ConfigItem(const ConfigItem &tmp);
		~ConfigItem();

	public:
		void clear();//释放空间
		ConfigItem& operator = (const ConfigItem& tmp);//赋值运算符重载
		void set_key(const char *tmp);//设置key
		char *get_key() const;//获取key
		void set_value(const char *tmp);//设置value
		char *get_value() const;//获取value
		int as_int() const;//将value的值以int类型返回
		double as_double() const;//将value的值以double类型返回
		string as_string() const;//将value的值以string类型返回（直接用char *赋值string类型变量即可）

	private:
		char *m_key;
		char *m_value;

		friend class Config;
	};


	/************************************
	@ ClassName		Config
	@ Brief:		配置文件类，用于读取配置文件，获取配置信息。
	@ Author:		czl
	@ Created: 		2019/01/09
	************************************/
	class Config
	{
	public:
		Config();
		Config(const Config &tmp);
		~Config();

	public:
		Config& operator = (const Config& tmp);
		void clear();//清空配置表
		int size() const;//获取配置表的大小
		int get_item(string key, ConfigItem *ptr);//获取某一个配置表中的元素
		void set_item(string key, const char *value);//设置并添加配置表中的元素
		ConfigItem &operator[](string key);//以数组下标的形式来获取配置项引用

	private:
		map<string, ConfigItem> m_item_map;//配置map
		ConfigItem *p_item;//配置项
	};

	int parse_ini_file(const char *fileName, Config *pconfig);//解析配置文件
}

#endif // !iniFile_parse
