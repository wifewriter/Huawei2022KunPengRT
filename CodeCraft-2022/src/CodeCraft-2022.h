#pragma once


#ifndef CODECRAFT_2022
#define CODECRAFT_2022

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include<set>
#include<algorithm>
#include<numeric>
using namespace std;
//一般数据格式内容
class DatasStruct {
public:
	vector<string> headName;
	vector<vector< string>> Datas;
};

//用户类
class User
{

private:
	string username;
public:
	//用户的不同时刻 节点分配带宽
	map<int, map<string, int>> Node_width;
	void SetUsername(string name);
	string GetUsername();
};



class Node
{
private:
	int Width;
	string NodeName;

public:

	map<int, int> Alloctime; //记录某一时刻分配权重



	int max_size;
	int Min_sum;
	set <string > UsefulUser;//记录该节点可分配用户


	//int Get_MinTime();//获得最大队列最小分配权重时刻
	//int Get_MinRate();//获得最大队列最小权重

	void SetTimes(int times); //设置时刻数
	void SetWidth(int width) { Width = width; };//设置带宽 
	void SetMaxSize(int size) { max_size = size; };//设置最大队列分配权重
	bool IsEffectNode(string username);//根据给出的用户 返回该节点是否可用
	void AssignedWidth(int time, int witdh, string username);//根据时刻指派带宽 并记录分配的用户
	void SetNondeName(string name);
};


#endif