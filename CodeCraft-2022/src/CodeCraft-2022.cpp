#include<random>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<set>
#include<algorithm>
#include<unordered_map>
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
	//用户的需求带宽
	int Need_width;
	//用户可用节点数（可变）
	int Usefulsize;
public:
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
	//不同时刻用户所需带宽
	vector<unordered_map<string, int>> WidthNeed;
public:
	vector<string>& Get_usersnames() { return Usernames; };
	void SetWidth(int time, int width, string name) { WidthNeed.resize(time + 1); WidthNeed[time][name] = width; }
	int GetWidth(int time, string name) {
		return WidthNeed[time][name];
	}
	void PushName(string name) { Usernames.push_back(name); };
};

class NodeManage
{
private:
	//所有用户节点
	vector<string> Nodenames;
	//存储目标节点下的可用用户名称
	unordered_map<string, vector<bool>> UsefulUser_flag;
	//存储node的带宽
	unordered_map<string, int> Node_Width;
public:
	vector<string>& Get_Nodenames() { return Nodenames; };
	void AddNode_Usefuluser(string nodename, bool flag) { UsefulUser_flag[nodename].push_back(flag); };
	vector<bool>& Get_UsefulUser(string nodename) { return UsefulUser_flag[nodename]; };
	void SetWidth(string name, int width) { Node_Width[name] = width; };
	int GetWidth(string name) {
		return Node_Width[name];
	}
	void PushName(string name) { Nodenames.push_back(name); };
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
void AverageChoose(vector<vector<int>>& allc, int user, int node)
{
	if (user < allc.size())
		NodeAssign(allc, user, node);
	else
		return;
	//如果没有分配完
	if (allc[user][0] != 0)
	{
		for (int j = node; j < allc[user].size(); j++)
		{
			if (allc[user][j] > 0)
			{
				//Reset后重新分配
				Reset(allc, user, j);
				//对该用户分配
				AverageChoose(allc, user, j);
				if (allc[user][0] == 0)
					break;
			}
		}
		return;
	}
	//分配完继续下一个
	else
	{
		AverageChoose(allc, user + 1, 1);
		return;
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

void upgrateUsefulNodeList(vector<vector<int>> &CountJuTemp,vector<int> &data){
    //跳过第一个列头
    for (int i = 1; i < data.size() ; ++i) {
        if(CountJuTemp[0][i] <= 0) data[i] = -1;
    }
}
/**
 *
 * @param CountJuTemp
 * @param data
 * @param val
 * @param nodeIndex
 * @return
 */
void averageDis(vector<vector<int>> &CountJuTemp, vector<int>& data,int val,vector<int> &nodeIndex){
    int temp = val;
    for (int index : nodeIndex) {
        if(CountJuTemp[0][index] > temp){
            data[index] += temp;
            data[0] -= temp;
            CountJuTemp[0][index] -= temp;
            temp = val;
        } else {
            //TODO:如果平均分配到最后都没有分配完，这里没有处理。
            int width = CountJuTemp[0][index];
            data[0] -= width;
            data[index] = width;
            temp = val + temp - CountJuTemp[0][index];
            CountJuTemp[0][index] = 0;
        }
    }
}
//时间节点上的分配
/**
 * 分配算法
 * @param Min_time
 * @param Max_time
 * @param Um
 * @param Nm
 * @param Flag =1 均分初始化，0 非均分初始化
 */
void DealOneAlg(int Min_time, int Max_time, UserManage* Um, NodeManage* Nm, bool Flag,ofstream &outfile)
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
				CountJu[j+1][i+1] = 0;
			else //不能分配节点置为-1
				CountJu[j+1][i+1] = -1;
		}
	}
	//时间的大循环
	for (int i = Min_time; i < Max_time; i++)
	{
		vector<vector<int>> CountJuTemp = CountJu;
		//获取当前时刻下的映射

		for (int k = 1; k < Usernames.size() + 1; k++)
		{

			//列头
			CountJuTemp[k][0] = Um->GetWidth(i, Usernames[k - 1]);
		}
		for (int j = 1; j < Nodenames.size() + 1; j++)
		{
			//行头
			CountJuTemp[0][j] = Nm->GetWidth(Nodenames[j - 1]);
		}
		int Reuslt = 1;

		//均分初始化
		if(Flag){
            for (auto &data : CountJuTemp) {
                if (data == CountJuTemp[0]) continue;
                //节点更新。
                upgrateUsefulNodeList(CountJuTemp,data);
                int nodeNum = 0,val =0;
                vector<int> nodeIndex;
                for (int j = 1;j <= data.size();j++) {
                    if(data[j] == 0){
                        nodeNum++;
                        nodeIndex.push_back(j);
                    }
                }
                if(nodeNum == 0) continue;
                val = data[0]/nodeNum;
                if(val == 0) continue;
                if(val == 3589){
                    int a =0;
                }
                averageDis(CountJuTemp,data,val,nodeIndex);
                if(data[0] < 0){
                    int a =0;
                }
            }
		}

		while (Reuslt != -1)
		{
			AverageChoose(CountJuTemp, Reuslt, 1);
			Reuslt = IsEffect(CountJuTemp);
		}

		//获得分配结果
		for (int i = 1; i < CountJuTemp.size(); i++)
		{
			outfile << Usernames[i - 1] << ":";
			string tempdata;
			for (int j = 1; j < CountJuTemp[i].size(); j++)
				if (CountJuTemp[i][j] > 0)
				{
					tempdata = tempdata + "<" + Nodenames[j - 1] + "," + to_string(CountJuTemp[i][j]) + ">" + ",";
				}
			if (tempdata.size() > 0)
				tempdata.pop_back();
			outfile << tempdata;
			outfile << endl;
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


	//获得需要调度的最大时刻数
	int Max_times = UserWidths.Datas.size();


	//初始化用户 
	for (int i = 1; i < PeopleQos.headName.size(); i++)
	{

		string username = PeopleQos.headName[i];
		Um->PushName(username);
	}

	//初始化node
	for (auto & Data : NodeWidths.Datas)
	{
		string Nodename = Data[0];
		Nm->PushName(Nodename);
		int NodeWidth = atoi(Data[1].c_str());
		Nm->SetWidth(Nodename, NodeWidth);
	}


	//初始化Node可用用户
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
		{
			//延迟小于阈值
			int this_Qos = stoi(PeopleQos.Datas[i][j].c_str());
			string Nodename = PeopleQos.Datas[i][0];
			if (this_Qos < qos)
			{
				Nm->AddNode_Usefuluser(Nodename, true);
			}
			else
				Nm->AddNode_Usefuluser(Nodename, false);
		}
	}


	//初始用户不同时刻所需化带宽
	for (int time = 0; time < UserWidths.Datas.size(); time++)
	{
		for (int j = 1; j < UserWidths.Datas[time].size(); j++)
		{
			string Username = UserWidths.headName[j];
			//获得该时刻下用户
			int width = atoi(UserWidths.Datas[time][j].c_str());
			//设置该时刻下用户所需带宽
			Um->SetWidth(time, width, Username);
		}
	}

    ofstream outfile(file_output);

    //运行调度算法
	int Min_time = 0;
	vector<int> Random5;
	//0-Max_times 随机5个时刻
    uniform_int_distribution<> values{1,Max_times};
    random_device rd;
    for (int k = 0; Random5.size() < 5; ++k) {
        default_random_engine rng {rd()};
        int t = values(rng);
        if(find(Random5.begin(),Random5.end(),t) == Random5.end()){
            Random5.push_back(t);
        }
    }
    sort(Random5.begin(),Random5.end(),less<int>());
    //95%时刻均分
    for (int l = 0; l < Random5.size(); ++l) {
        int end = Random5[l];
        DealOneAlg(Min_time, end, Um, Nm,true,outfile);
        DealOneAlg(end,end+1,Um,Nm, false,outfile);
        Min_time = end+1;
        if(l == Random5.size()-1){
            //最后一个区间
            DealOneAlg(Min_time,Max_times,Um,Nm, true,outfile);
        }
    }

	return 0;
}