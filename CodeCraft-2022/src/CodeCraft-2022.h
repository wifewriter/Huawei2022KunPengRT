#pragma once


#ifndef CODECRAFT_2022
#define CODECRAFT_2022

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
using namespace std;
//一般数据格式内容
class DatasStruct {
public:
	vector<string> headName;
	vector<vector< string>> Datas;
};

//用户类
class USer
{
private:
    string Username;
    map<string,int> NodeWidth; //用户带宽需求
    map<string,int> Qosnode;//用户最大时延
public:
    USer();
    ~USer();
    void SetUseranme(string name); //设置用户名
    void SetNodeWith(string ,int); //设置带宽需求
    void SetQosnodes();//设置时延
};
//节点类
class Node
{
private:
    string NodeName;//节点名称
    int Max_Qos;//最大负载
public:
    Node();
    ~Node();
    void SetNodeName(string &);//设置节点名称
    void SetMax_Qos(int);//设置最大负载
};
//用户管理类
class UserManage
{
private:
    map<string,USer> UserDatas;
public:
    UserManage();
    ~UserManage();
    USer * GetUser(string &);
    void AppendUser(USer &);
};

//节点管理类
class NodeManage
{
private:
    map<string,Node> UserDatas;
public:
    NodeManage();
    ~NodeManage();
    Node * GetNode(string &);
    void AppendNode(Node &);
};

#endif