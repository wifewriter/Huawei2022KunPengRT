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
	//用户的需求分配结果
	unordered_map<string, int> Node_width;
	//用户的需求带宽
	int Need_width;
	//用户可用节点数（可变）
	int Usefulsize;
public:
	
	//设置分配结果
	void Set_Width(string& nodename, int width) { Node_width[nodename] = width; };
	//获得目标节点分配结果
	int Get_Width(string& nodename) { return Node_width[nodename]; };
	//增加、减少目标分配结果
	void ADDWidth(string& nodename, int width) { Node_width[nodename] += width; Need_width -= width; };
	//设置当前用户可用节点数量
	void Set_UsefulSize(int size) { Usefulsize = size; };
	//获得当前用户可用节点数量
	int Get_UsefulSize() { return Usefulsize; };
	//获得用户当前时刻下带宽需求
	int GetNeed_width() { return Need_width; };
	//设置用户当前时刻下带宽需求
	void SetNeed_width(int width) { Need_width = width; };
};


//节点类
class Node
{
private:
	//当前最大带宽
	int MaxWidth;

public:
	//设置最大带宽 
	void SetMaxWidth(int width) { MaxWidth = width; };
	//获得当前带宽
	int GetWidth() { return MaxWidth; };
	//修改当前可用带宽
	void ADDWidth(int width) { MaxWidth -= width; };

};


class UserManage
{
private:
	//所有用户名
	vector<string> Usernames;
	//不同时刻下的用户
	vector<unordered_map<string, User *>> Time_User; 
public:
	//设置最大存储时刻
	void InitTimes(int time) { Time_User.resize(time); } 
	//获取该时刻下的指定用户
	User* GetUser(string& name,int time) { return Time_User[time][name]; };
	//获取该时刻下所有用户
	unordered_map<string, User *>* GetUserMap(int time) { return &Time_User[time]; };
	//将所有时刻下的用户初始化
	void Append_User(User* user, string name) 
	{
		for (int i = 0; i < Time_User.size(); i++)
		{
			User * time_user = new User;
			*time_user = *user;
			Time_User[i][name] = time_user;
		}
		Usernames.push_back(name);
	};
	vector<string>& Get_usersnames() { return Usernames; };
};

class NodeManage
{
private:
	//所有用户节点
	vector<string> Nodenames;
	//不同时刻下的节点
	vector<unordered_map<string, Node*>> Time_Node;
	//存储目标节点下的可用用户名称
	unordered_map<string, vector<bool>> UsefulUser_flag;
public: 
	Node* GetNode(string& name,int time) { return Time_Node[time][name]; };
	//获取该时刻下的所有节点
	unordered_map<string, Node*>* GetNodeMap(int time) { return &Time_Node[time]; };
	void InitTimes(int time) { Time_Node.resize(time); };
	void Append_Node(Node* node, string name) 
	{ 
		for (int i = 0; i < Time_Node.size(); i++)
		{
			Node* time_node = new Node;
			*time_node = *node;
			Time_Node[i][name] = time_node;
		}
		Nodenames.push_back(name);
	};
	vector<string>& Get_Nodenames() { return Nodenames; };
	void AddNode_Usefuluser(string nodename, bool flag) { UsefulUser_flag[nodename].push_back(flag); };
	vector<bool>& Get_UsefulUser(string nodename) { return UsefulUser_flag[nodename]; };
};



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

//获取结构体数据
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

//针对一个用户的分配
void NodeAssign(vector<vector<int>>& allc, int user, int node)
{
	
	for (int j = node; j < allc[user].size(); j++)
	{
		//如果不能分配
		if (allc[user][j] == -1)
			continue;
		else
		{
			//节点可分配为0
			if (allc[0][j] == 0)
				continue;
			//用户需求小于等于可分配
			if (allc[user][0] <= allc[0][j])
			{
				allc[user][j] += allc[user][0];
				allc[0][j] -= allc[user][0];
				allc[user][0] -= allc[user][0];
				
			}
			//用户需求大于可分配
			else 
			{
				//该分配所有带宽
				//更新所分配位置
				allc[user][j] += allc[0][j];
				int temp = allc[user][0];
				//更新用户需求
				allc[user][0] -= allc[0][j];
				//更新节点带宽
				allc[0][j] -= allc[0][j];
			}
		}
	}
	
}
//重新分配
void Reset(vector<vector<int>>& allc, int user, int node)
{
	//减少分配
	int Rest_width = allc[user][0];
	for (int i = 1; i < user; i++)
	{
		//如果可分配
		if (allc[i][node] != -1)
		{
			//如果已分配大于需求
			if (allc[i][node] > Rest_width)
			{
				allc[i][node] -= Rest_width;
				allc[i][0] += Rest_width;
				Rest_width -= Rest_width;
				
			}
			//如果已分配小于需求
			else
			{
				Rest_width -= allc[i][node];
				allc[i][node] -= allc[i][node];
			}
		}	
	}
}
//迭代分配 贪心算法 user对目标用户分配，node 对目标节点分配
void AverageChoose(vector<vector<int>> & allc,int user,int node)
{
	if(user < allc.size())
		NodeAssign(allc, user, node);
	else
		return;
	//如果没有分配完
	if (allc[user][0] != 0)
	{
		for (int j = 1; j < allc[user].size(); j++)
		{
			//Reset后重新分配
			Reset(allc, user, j);
			//对该用户分配
			AverageChoose(allc, user, j);
			if (allc[user][0] == 0)
				break;
		}
	}
	//分配完继续下一个
	else
	{
		AverageChoose(allc, user+1, node);
	}
	


}
int IsEffect(vector<vector<int>>& CountJu)
{
	for (int i = 0; i < CountJu.size(); i++)
	{
		if (CountJu[i][0] != 0)
			return i;
	}
	return -1;
}


//时间节点上的分配 
void DealOneAlg(int Min_time, int Max_time, UserManage * Um, NodeManage *Nm)
{
	//初始化计算矩阵
	vector<string>Usernames = Um->Get_usersnames();
	vector<string>Nodenames = Nm->Get_Nodenames();
	vector<vector<int>> CountJu(Usernames.size() + 1, vector<int>(Nodenames.size() + 1));
	//行头 用户所需带宽
	//列头 Node所拥有的带宽


	//初始化矩阵分配数值 不能分配为-1
	for (int i = 0; i < Nodenames.size(); i++)
	{
		vector<bool> flags = Nm->Get_UsefulUser(Nodenames[i]);
		for (int j = 0; j < flags.size(); j++)
		{
			if (flags[j])
				CountJu[j][i] = 0;
			else //不能分配节点置为-1
				CountJu[j][i] = -1;
		}
	}
	for (int i = Min_time; i < Max_time; i++)
	{
		vector<vector<int>> CountJuTemp = CountJu;
		//获取当前时刻下的映射
		unordered_map<string, Node *>* All_Node = Nm->GetNodeMap(i);
		unordered_map<string, User *>* All_User = Um->GetUserMap(i);
		
		for (int i = 1; i < Usernames.size()+1; i++)
		{
			
			//列头
			CountJuTemp[i][0] = (*All_User)[Usernames[i-1]]->GetNeed_width();
		}
		for (int j = 1; j < Nodenames.size()+1; j++)
		{
			//行头
			CountJuTemp[0][j] = (*All_Node)[Nodenames[j-1]]->GetWidth();
		}
		int Reuslt = 1;
		while (Reuslt != -1)
		{
			AverageChoose(CountJuTemp, Reuslt, 1);
			Reuslt = IsEffect(CountJuTemp);
		}
		//获得分配结果
		for (int i = 1; i < CountJuTemp.size(); i++)
		{
			for (int j = 1; j < CountJuTemp[i].size(); j++)
				if(CountJuTemp[i][j] != -1)
				cout << "用户 :" << Usernames[i - 1] << "   节点 :" << Nodenames[j - 1] << "  分配: " << CountJuTemp[i][j] <<  endl;
		}

	}
	return;
}
int main()
{

	//获取客户宽带需求
	DatasStruct UserWidths = GetData(file_root + file_Demand);

	//获取边缘节点带宽数
	DatasStruct NodeWidths = GetData(file_root + file_Bandwidth);

	//获取客户与边缘节点网络时延
	DatasStruct PeopleQos = GetData(file_root + file_qos);

	//获取config
	int qos = ReadQos(file_root + file_config);

	UserManage* Um = new UserManage;
	NodeManage* Nm = new NodeManage;


	//将不同node user的共同参数初始化
	unordered_map<string, User*>AllUser;
	unordered_map<string, Node*>AllNode;
	//获得需要调度的最大时刻数
	int Max_times = UserWidths.Datas.size();

	Um->InitTimes(Max_times);
	Nm->InitTimes(Max_times);
	//初始化用户 
	for (int i = 1; i < PeopleQos.headName.size(); i++)
	{
		User* New_user = new User;
		string username = PeopleQos.headName[i];
		AllUser[username] = New_user;
	}

	//初始化node
	for (int i = 0; i < NodeWidths.Datas.size(); i++)
	{
		Node* new_node = new Node;
		string Nodename = NodeWidths.Datas[i][0];
		new_node->SetMaxWidth(atoi(NodeWidths.Datas[i][1].c_str()));
		AllNode[Nodename] = new_node;
	}



	//初始化Node可用用户
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
		{
			//延迟小于阈值
			int this_Qos = atoi(PeopleQos.Datas[i][j].c_str());
			string Nodename = PeopleQos.Datas[i][0];
			if (this_Qos < qos)
			{
				
				Nm->AddNode_Usefuluser(Nodename, true);
			}
			else
				Nm->AddNode_Usefuluser(Nodename, false);
		}
	}

	//初始化User管理 和 Node管理
	for (auto i = AllNode.begin(); i != AllNode.end(); i++)
		Nm->Append_Node(i->second, i->first);
	for (auto i = AllUser.begin(); i != AllUser.end(); i++)
		Um->Append_User(i->second, i->first);
	//初始化用户不同时刻所需带宽
	for (int time = 0; time < UserWidths.Datas.size(); time++)
	{
		for (int j = 1; j < UserWidths.Datas[time].size(); j++)
		{
			string Username = UserWidths.headName[j];
			//获得该时刻下用户
			User* this_user = Um->GetUser(Username, time);
			int width = atoi(UserWidths.Datas[time][j].c_str());
			//设置该时刻下用户所需带宽
			this_user->SetNeed_width(width);
		}
	}

	//运行调度算法
	int Min_time = 0;

	DealOneAlg(Min_time, Max_times, Um, Nm);
	return 1;
}