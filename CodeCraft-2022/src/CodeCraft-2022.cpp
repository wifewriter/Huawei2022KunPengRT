#include<random>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include<set>
#include<algorithm>
#include<numeric>
#include<unordered_map>
#include<thread>
using namespace std;



using namespace std;
//作品提交路径
string file_root = "/data/";
string file_Demand = "demand.csv";
string file_Bandwidth = "site_bandwidth.csv";
string file_qos = "qos.csv";
string file_config = "config.ini";
string file_output = "/output/solution.txt";





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
	//vector<int, unordered_map<string, int>> Node_width;
	vector<unordered_map<string,int>> Node_width;
	void SetUsername(string name);
	string GetUsername();
	set<string> UsefulNode;
	void InitUsertime(int times);
	unordered_map<string, int>* getTimedata(int time) { return &Node_width[time]; };
};



class Node
{
private:
	int Width;
	string NodeName;

public:
	unordered_map<int, int> Alloctime; //记录某一时刻分配权重
	vector<int> UsefulUser_index; //可用用户节点序号
	int max_size;
	int Min_sum;
	set <string > UsefulUser;//记录该节点可分配用户


	//int Get_MinTime();//获得最大队列最小分配权重时刻
	//int Get_MinRate();//获得最大队列最小权重

	void SetTimes(int times); //设置时刻数
	void SetWidth(int width) { Width = width; };//设置带宽 
	void SetMaxSize(int size) { max_size = size; };//设置最大队列分配权重
	bool IsEffectNode(string &username);//根据给出的用户 返回该节点是否可用
	void AssignedWidth(int &time, int &witdh, string &username);//根据时刻指派带宽 并记录分配的用户
	void SetNondeName(string &name);
	string GetNodeName() { return NodeName; };
};

//*************************************全局变量******************************************//
unordered_map<string, User* >Alluser;
unordered_map<string, Node*> AllNodes;
string output_data;
//*************************************全局变量******************************************//

void User::SetUsername(string name)
{
	username = name;
}
string User::GetUsername()
{
	return username;
}
void User::InitUsertime(int times)
{
		Node_width.resize(times);
}

void Node::SetTimes(int times)
{
	for (int i = 0; i < times; i++)
	{
		Alloctime[i] = Width;
	}
}

void  Node::SetNondeName(string& name)
{
	NodeName = name;
}
bool Node::IsEffectNode(string& username)
{
	if (UsefulUser.find(username) != UsefulUser.end())
		return true;
	return false;
}

void Node::AssignedWidth(int& time, int& width, string& username)
{

	Alloctime[time] -= width;
	User* Target_user = Alluser[username];
	//if (Target_user != nullptr)
	//	Target_user->Node_width[time][NodeName] = width;

}
//字符串切割方法
std::vector<std::string> stringSplit(const std::string& str, char delim) {
	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}

//去除换行符
void strim(string& str)
{
	int Pos = str.size() - 1;
	if (str[Pos] == '\n' || str[Pos] == '\r')
	{
		str.erase(str.begin() + Pos);
	}
}

DatasStruct GetData(string filepath)
{
	DatasStruct tempData;
	ifstream infile(filepath);
	vector<string> data;
	vector<string> head;
	bool Head = true;
	while (infile.good())
	{

		string value;
		getline(infile, value);
		data = stringSplit(value, ',');
		if (Head)
		{
			strim(data[data.size() - 1]);
			tempData.headName = data;
			Head = false;
		}
		else
		{
			if (data.size() != 0)
				tempData.Datas.push_back(data);
		}

		//cout << value << endl;
	}
	return tempData;
}

//获取有效分配
int GetEffectSummer()
{
	return 1;
}
//随机选择目标数组中的n个数据 保证为uniform
vector<int> Random_Choose(vector<int>& Datas, int n)
{
	int count = 0;
	vector<int> Radoms;
	int Point = Datas.size() - 1;
	while (count < 5)
	{
		int chosIndex = rand() % (Point + 1);
		if (chosIndex < Point)
		{
			Radoms.push_back(Datas[chosIndex]);
			Datas[chosIndex] = Datas[Point];
			Point--;
		}
		else if (chosIndex == Point)
		{
			Radoms.push_back(Datas[Point]);
			Point--;
		}
		count++;
	}
	return Radoms;
}
//选择一个节点
void ChooseNode(int time, vector<string>& One_line, vector<string>& Usernames)
{
	//全分配法
	for (int k = 1; k < One_line.size(); k++)
	{
		int People_Require = atoi(One_line[k].c_str());
		string Username = Usernames[k];
		//output_data = output_data + Username + ":";
		User* this_user = Alluser[Username];
		int rand_choose = rand() % this_user->UsefulNode.size();
		set<string> UsefulNode = this_user->UsefulNode;
		while (People_Require != 0)
		{
			set<string>::iterator it(UsefulNode.begin());
			advance(it, rand_choose);
			string Nodename = *it;
			Node* Now_Node = AllNodes[Nodename];
			if (Now_Node->Alloctime[time] > People_Require)
			{
				Now_Node->AssignedWidth(time, People_Require, Username);
				//output_data = output_data + "<" + Now_Node->GetNodeName() + "," + to_string(People_Require) + ">";
				this_user->Node_width[time][Now_Node->GetNodeName()] += People_Require;
				People_Require = 0;
				break;
			}
			else if (Now_Node->Alloctime[time] != 0 && Now_Node->Alloctime[time] < People_Require)
			{
				Now_Node->AssignedWidth(time, Now_Node->Alloctime[time], Username);
				People_Require -= Now_Node->Alloctime[time];
				//output_data = output_data + "<" + Now_Node->GetNodeName() + "," + to_string(People_Require) + ">" + ",";
				this_user->Node_width[time][Now_Node->GetNodeName()] += People_Require;
			}
			rand_choose = (rand_choose + 1) % UsefulNode.size();
		}
		//output_data = output_data + "\r\n";
	}


}

//平均分配法
void AverageChoose(int time, vector<string>& One_line, vector<string>& Usernames)
{
	vector<unordered_map<User*, int> >Rest_Width; //记录当前时刻的余留带宽
	for (auto tNode = AllNodes.begin(); tNode != AllNodes.end(); tNode++)
	{
		vector<int> Usefulpeople = tNode->second->UsefulUser_index;
		Node * This_node = tNode->second;
		string NodeName = tNode->second->GetNodeName();

		for (int i = 0; i < Usefulpeople.size(); i++)
		{
			int peopele_index = Usefulpeople[i];
			User* this_user = Alluser[Usernames[peopele_index]]; //获取当前用户
			int People_Require = atoi(One_line[peopele_index].c_str());
			//获取当前用户可用节点数
			int Node_size = this_user->UsefulNode.size();
			//获得每个节点平均取值
			int Width = People_Require / Node_size;
			//余下带宽
			int res_width = People_Require % Node_size;
			//将余留带宽加入剩余带宽容器
			if (res_width != 0)
			{
				unordered_map<User*, int> New_res;
				New_res[this_user] = res_width;
				Rest_Width.push_back(New_res);
			}

			//将该时刻用户的输出
			unordered_map<string, int>* Pp = &(this_user->Node_width[time]);
			//考虑该均值带宽是否满足最大带宽要求
			int Node_Width = This_node->Alloctime[time];
			//当带宽小于均值时
			if (Node_Width < Width)
			{
				This_node->Alloctime[time] -= Node_Width;
				unordered_map<User*, int> New_res;
				New_res[this_user] = Width - Node_Width;
				Rest_Width.push_back(New_res);
				(*Pp)[NodeName] += Node_Width;
			}
			//当带宽大于等于均值时
			else if (Node_Width >= Width)
			{
				This_node->Alloctime[time] -= Width;
				(*Pp)[NodeName] += Width;
				//顺便考虑将余下带宽使用
				if (This_node->Alloctime[time] != 0)
				{
					for (int res_size = Rest_Width.size() - 1; res_size >= 0; res_size--)
					{
						unordered_map<User*, int> this_res = Rest_Width[res_size];
						User* Rest_User = this_res.begin()->first;
						int Res_Widths = this_res.begin()->second;
						//如果余下的带宽比该节点的带宽小
						if (Res_Widths <= This_node->Alloctime[time])
						{
							This_node->Alloctime[time] -= Res_Widths;
							Rest_User->Node_width[time][NodeName] += Res_Widths; //分配完毕 删除该节点
							Rest_Width.pop_back();
						}
						//如果余下节点的带宽比该节点带宽大 分配部分
						else if (Res_Widths > This_node->Alloctime[time])
						{
							This_node->Alloctime[time] -= This_node->Alloctime[time];
							Rest_User->Node_width[time][NodeName] += This_node->Alloctime[time];
							this_res.begin()->second -= This_node->Alloctime[time];
						}
					}
				}

			}

		}
	}

	
}
//读取ini文件中的配置
int ReadQos(string filepath)
{
	DatasStruct iniData = GetData(filepath);
	string Qos = iniData.Datas[0][0];
	int position = Qos.find("=");
	string Result;
	if (position != Qos.npos)
	{
		Result = Qos.substr(position + 1, Qos.size());
	}
	return atoi(Result.c_str());
}



void DealOneAlg(int min_index, int max_index, vector<vector<string>>& Datas, vector<string>& Usernames, unordered_map<int, bool>& MaxTimesOrNot)
{
	for (int i = min_index; i < max_index; i++)
	{

		if (MaxTimesOrNot[i] == true)
		{
			ChooseNode(i, Datas[i], Usernames);
		}
		//调用其他分配方案
		else
		{
			AverageChoose(i, Datas[i], Usernames);
		}
	}

	return;
}
void test()
{
	return;
}

void Out(int NeedTimes)
{
	//输出结果
	ofstream outfile(file_output);

	int Nowtime = 0;
	while (Nowtime < NeedTimes)
	{
		for (auto user = Alluser.begin(); user != Alluser.end(); user++)
		{
			outfile << user->second->GetUsername() << ":";
			unordered_map<string, int>* PPt = user->second->getTimedata(Nowtime);
			int count = 0;
			for (auto node = PPt->begin(); node != PPt->end(); node++)
			{
				outfile << "<" << node->first << "," << node->second << ">";
				if (count < PPt->size() - 1)
				{
					outfile << ",";
				}
				count++;
			}
			outfile << endl;
		}
		Nowtime++;
	}
	outfile.close();
}
int main() {

	//获取客户宽带需求
	DatasStruct UserWidths = GetData(file_root + file_Demand);

	//获取边缘节点带宽数
	DatasStruct NodeWidths = GetData(file_root + file_Bandwidth);

	//获取客户与边缘节点网络时延
	DatasStruct PeopleQos = GetData(file_root + file_qos);


	//获取config
	int qos = ReadQos(file_root + file_config);


	//初始化用户
	for (int i = 1; i < UserWidths.headName.size(); i++)
	{
		User* New_user = new User;

		New_user->SetUsername(UserWidths.headName[i]);
		Alluser[UserWidths.headName[i]] = New_user;
	}

	//获取需要分配时间的数量
	int NeedTimes = UserWidths.Datas.size();
	int MaxTimes = NeedTimes * 0.05;

	//初始化用户可用节点
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
			//延迟小于阈值
			if (atoi(PeopleQos.Datas[i][j].c_str()) < qos)
			{
				Alluser[PeopleQos.headName[j]]->UsefulNode.insert(PeopleQos.Datas[i][0]);
				Alluser[PeopleQos.headName[j]]->InitUsertime(NeedTimes);
			}
	}

	//初始化node
	for (int i = 0; i < NodeWidths.Datas.size(); i++)
	{
		Node* new_node = new Node;
		new_node->max_size = NeedTimes * 0.05; //最大可分配节点数
		new_node->SetWidth(atoi(NodeWidths.Datas[i][1].c_str()));
		new_node->SetNondeName(NodeWidths.Datas[i][0]);
		// cout<<to_string(NodeWidths.Datas[i][0].size())<<endl;
		new_node->SetTimes(NeedTimes);
		
		AllNodes[NodeWidths.Datas[i][0]] = new_node;

	}

	//初始化Node可用用户
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
			//延迟小于阈值
			if (atoi(PeopleQos.Datas[i][j].c_str()) < qos)
			{
				AllNodes[PeopleQos.Datas[i][0]]->UsefulUser.insert(PeopleQos.headName[j]);
				AllNodes[PeopleQos.Datas[i][0]]->UsefulUser_index.push_back(j);
			}
	}

	srand((unsigned)time(0));
	vector<int> Times;
	unordered_map<int, bool> MaxTimesOrNot;//存储是否为最大5%分配节点
	//初始化时间分配节点
	for (int i = 0; i < NeedTimes; i++)
	{
		Times.push_back(i);
		MaxTimesOrNot[i] = false;
	}
	vector<int> TargetTimes = Random_Choose(Times, MaxTimes);
	for (int i = 0; i < TargetTimes.size(); i++)
	{
		MaxTimesOrNot[TargetTimes[i]] = true;
	}


	//对时间戳进行2分处理
	int min_time = 0;
	int max_time = UserWidths.Datas.size() ;
	int mid_time = (min_time + max_time) / 2;
	// // thread b(test);
	// // b.join();
	// thread a(DealOneAlg,min_time,mid_time,std::ref(UserWidths.Datas),std::ref(UserWidths.headName),std::ref(MaxTimesOrNot));
	// thread b(DealOneAlg,mid_time,max_time+1,std::ref(UserWidths.Datas),std::ref(UserWidths.headName),std::ref(MaxTimesOrNot));
	// a.join();
	// b.join();

	time_t start_time(NULL);
	DealOneAlg(0, max_time, UserWidths.Datas, UserWidths.headName, MaxTimesOrNot);
	time_t end_time(NULL);

	cout << "算法时间:" << end_time - start_time << endl;
	
	//输出文件
	Out(NeedTimes);

	for (auto i = AllNodes.begin(); i != AllNodes.end(); i++)
	{
		delete (*i).second;
	}

	for (auto i = Alluser.begin(); i != Alluser.end(); i++)
	{
		delete (*i).second;
	}
	return 0;
}
