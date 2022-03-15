
#include"config.h"
#include"CodeCraft-2022.h"


using namespace std;
string file_root = "data/";
string file_Demand = "demand.csv";
string file_Bandwidth = "site_bandwidth.csv";
string file_qos = "qos.csv";
string file_config = "config.ini";
string file_output = "output.txt";

//*************************************全局变量******************************************//
map<string, User* >Alluser;
map<string, Node*> AllNodes;
//*************************************全局变量******************************************//

void User::SetUsername(string name)
{
	username = name;
}
string User::GetUsername()
{
	return username;
}


void Node::SetTimes(int times)
{
	for (int i = 0; i < times; i++)
	{
		Alloctime[i] = Width;
	}
}

void  Node::SetNondeName(string name)
{
	NodeName = name;
}
bool Node::IsEffectNode(string username)
{
	if (UsefulUser.find(username) != UsefulUser.end())
		return true;
	return false;
}

void Node::AssignedWidth(int time, int width, string username)
{
	Alloctime[time] -= width;
	User* Target_user = Alluser[username];
	if (Target_user != nullptr)
		Target_user->Node_width[time][NodeName] = width;

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
//选择一个节点
void ChooseNode(vector<string>& One_line, int time, vector<string> Usernames)
{
	//简单分配法
	for (int k = 1; k < One_line.size(); k++)
	{
		int People_Require = atoi(One_line[k].c_str());
		//采用最简单分配法
		for (auto i = AllNodes.begin(); i != AllNodes.end(); i++)
		{
			Node* Now_Node = (*i).second;
			bool EffectNode = Now_Node->IsEffectNode(Usernames[k]);
			if (!EffectNode )
				continue;
			if (People_Require == 0)
				break;
			//如果分配的时间小于带宽 并且 存在于可分配集合中
			if (Now_Node->Alloctime[time] > People_Require && EffectNode)
			{
				Now_Node->AssignedWidth(time, People_Require, Usernames[k]);
				People_Require = 0;
				break;
			}
			else if (Now_Node->Alloctime[time] != 0 && Now_Node->Alloctime[time] < People_Require && EffectNode)
			{
				Now_Node->AssignedWidth(time, Now_Node->Alloctime[time], Usernames[k]);
				People_Require -= Now_Node->Alloctime[time];
			}
		}
	}


	//如果最大队列数据长度比它小 直接分配进入
	//如果最大队列数据长度已满 剔除最小时间下的分配到常规分配， 将该分配放入最大分配
}

int main() {

	//获取客户宽带需求
	DatasStruct UserWidths = GetData(file_root + file_Demand);

	//获取边缘节点带宽数
	DatasStruct NodeWidths = GetData(file_root + file_Bandwidth);

	//获取客户与边缘节点网络时延
	DatasStruct PeopleQos = GetData(file_root + file_qos);


	//获取config
	using namespace iniFile_parse;
	Config config;
	int ret = 0;
	ret = parse_ini_file((file_root + file_config).c_str(), &config);
	if (ret != EC_NONE)
	{
		cout << "ERROR | parse the ini file failed.";
		return EC_ERROR;
	}
	int qos = config["qos_constraint"].as_int();



	//初始化用户
	for (int i = 1; i < UserWidths.headName.size(); i++)
	{
		User* New_user = new User;
		New_user->SetUsername(UserWidths.headName[i]);
		Alluser[UserWidths.headName[i]] = New_user;
	}

	//获取需要分配时间的数量
	int NeedTimes = UserWidths.Datas.size();
	int MaxTimes = NeedTimes * 0.95;


	//初始化node
	for (int i = 0; i < NodeWidths.Datas.size(); i++)
	{
		Node* new_node = new Node;
		new_node->max_size = MaxTimes;
		new_node->SetWidth(atoi(NodeWidths.Datas[i][1].c_str()));
		new_node->SetNondeName(NodeWidths.Datas[i][0]);
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
			}
	}

	//节点的分配数据
	for (int i = 0; i < UserWidths.Datas.size(); i++)
	{
		//选择节点分配
		ChooseNode(UserWidths.Datas[i], i, UserWidths.headName);	
	}


	//输出结果
	ofstream outfile(file_root + file_output);
	int times = 0;
	while (times < NeedTimes)
	{
		for (auto i = Alluser.begin(); i != Alluser.end(); i++)
		{
			User* TargetUSer = (*i).second;
			string one_string = TargetUSer->GetUsername() + ":";
			map<int, map<string, int>> data = TargetUSer->Node_width;

	
			map<string, int> Rdata = data[times];
			for (auto k = Rdata.begin(); k != Rdata.end(); k++)
			{

				one_string = one_string + "<" + (*k).first + "," + to_string((*k).second) + ">";
				
			}
			outfile << one_string.c_str();
			outfile << "\r\n";
		}
		times++;
	}
	outfile.close();
	return 0;
}
