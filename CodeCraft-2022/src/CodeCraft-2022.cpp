#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<set>
#include<algorithm>
#include<unordered_map>
using namespace std;

//一般数据格式内容
class DatasStruct {
public:
	vector<string> headName;
	vector<vector< string>> Datas;
};

class UserManage
{
private:
	//所有用户名
	vector<string> Usernames;
    //不同时刻用户所需带宽
    vector<unordered_map<string, int>> WidthNeed;
    //每个用户名对应的可使用的节点数目
    unordered_map<string,int> userToNodeNum;
public:
    vector<string>& Get_usersnames() { return Usernames; };
	void SetWidth(int time, int width, const string& name) { WidthNeed.resize(time + 1); WidthNeed[time][name] = width; }
	int GetWidth(int time, const string& name) {
		return WidthNeed[time][name];
	}
	void PushName(const string& name) { Usernames.push_back(name); };
    vector<unordered_map<string, int>>& getWidthNeed(){
        return this->WidthNeed;
    }
    vector<string>& getUserNames(){
        return this->Usernames;
    }
    void setUserToNodeNum(const string& userName){
        if(this->userToNodeNum[userName]){
            this->userToNodeNum[userName] +=1;
        } else{
            this->userToNodeNum[userName] = 1;
        }
    }
    int getUserToNodeNum(const string &name){
        return this->userToNodeNum[name];
    }
};

class NodeManage
{
private:
	//所有用户节点
	vector<string> Nodenames;
	//节点与下表索引
	unordered_map<string,int> nameToIndexMap;
	//存储目标节点下的可用用户名称
	unordered_map<string, vector<bool>> UsefulUser_flag;
	//存储node的带宽
	unordered_map<string, int> Node_Width;
	//存储节点的载荷序列 前5%大 和 时间
	unordered_map<string ,vector<vector<int>>> payload_List_map;
public:
	vector<string>& Get_Nodenames() { return Nodenames; };
	void AddNode_Usefuluser(string nodename, bool flag) { UsefulUser_flag[nodename].push_back(flag); };
	vector<bool>& Get_UsefulUser(string nodename) { return UsefulUser_flag[nodename]; };
	void SetWidth(string name, int width) { Node_Width[name] = width; };
	int GetWidth(string name) {
		return Node_Width[name];
	}
	void PushName(string name) { Nodenames.push_back(name); };
	void GetPre5percentPayloadList(vector<unordered_map<string, int>> &widthNeed,vector<string>&userNames,int max);
	void setNameToIndexMap();
    unordered_map<string ,vector<vector<int>>> &getPayLoadListMap(){
        return this->payload_List_map;
    };
    int getNameToIndexMap(const string &name){
        return this->nameToIndexMap[name];
    }
};
//获取前5%大。
void NodeManage::GetPre5percentPayloadList(vector<unordered_map<string, int>> &widthNeed,vector<string>& userNames,int max){
    for (auto &i :this->UsefulUser_flag) {
        vector<vector<int>> totalPayload;
        for (int k = 0;k<widthNeed.size();k++) {
            int perNodeSum = 0;
            for (int j = 0; j <widthNeed[k].size() ; ++j) {
                if(i.second[j] == 1){
                    perNodeSum += widthNeed[k][userNames[j]];
                }
            }
            totalPayload.push_back(vector<int>{perNodeSum,k});
        }
        sort(totalPayload.begin(),totalPayload.end(),greater<vector<int>>());
        //一个节点
        this->payload_List_map[i.first] = vector<vector<int >>(totalPayload.begin(),totalPayload.begin()+max*0.05);
    }

}
//TODO:这里由于矩阵前有一列头，这里下标索引需要+1；
void NodeManage::setNameToIndexMap() {
    for (int i = 0; i < Nodenames.size(); ++i) {
        this->nameToIndexMap[Nodenames[i]] = i+1;
    }
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
        if(CountJuTemp[0][i] <= 0 and data[i] == 0) data[i] = -1;//TODO:这里data[i]如果分配了的话就不重置。
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
            data[index] += width;
            temp = val + temp - CountJuTemp[0][index];
            CountJuTemp[0][index] -= width;
        }
    }
}

int main()
{
    ofstream outfile("/output/solution.txt");
    DatasStruct UserWidths = GetData("/data/demand.csv");
	//获取边缘节点带宽数
	DatasStruct NodeWidths = GetData( "/data/site_bandwidth.csv");
	//获取客户与边缘节点网络时延
	DatasStruct PeopleQos = GetData("/data/qos.csv");

	//获取config
	int qos = ReadQos("/data/config.ini");

	auto* Um = new UserManage;
	auto* Nm = new NodeManage;

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
		int NodeWidth = stoi(Data[1].c_str());
		Nm->SetWidth(Nodename, NodeWidth);
	}
	//初始化Node可用用户
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
		{
			//延迟小于阈值
			int this_Qos = stoi(PeopleQos.Datas[i][j]);
			string Nodename = PeopleQos.Datas[i][0];
            Nm->AddNode_Usefuluser(Nodename, this_Qos < qos);
		}
	}
	//初始用户不同时刻所需化带宽
	for (int time = 0; time < UserWidths.Datas.size(); time++)
	{
		for (int j = 1; j < UserWidths.Datas[time].size(); j++)
		{
			string Username = UserWidths.headName[j];
			//获得该时刻下用户
			int width = stoi(UserWidths.Datas[time][j].c_str());
			//设置该时刻下用户所需带宽
			Um->SetWidth(time, width, Username);
		}
	}
	//更新每个节点的分配矩阵
    Nm->GetPre5percentPayloadList(Um->getWidthNeed(),Um->getUserNames(),Max_times);
    Nm->setNameToIndexMap();

    //分配策略---begin
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
            if (flags[j]){
                CountJu[j+1][i+1] = 0;
                Um->setUserToNodeNum(Um->getUserNames()[j]);
            }
            else //不能分配节点置为-1
                CountJu[j+1][i+1] = -1;
        }
    }

    //时间的大循环
    for (int i = 0; i < Max_times; i++)
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
        //判断当前时间点哪些node需要“全力输出”，其余node尽可能少
        for (auto &l : Nm->getPayLoadListMap()) {
            for (auto &k : l.second ) {
                if(k[0] <= 0) break;
                if(k[1] == i){
                    //当前节点为fireOut节点、尽可能全力分配载荷下去。
                    int colIndex = Nm->getNameToIndexMap(l.first); //获取列下标
                    //获取能分配用户节点的所有矩阵下标
                    vector<int > userVector{};
                    for (int j = 0; j <CountJuTemp.size() ; ++j) {
                        if(CountJuTemp[j][colIndex] == 0){
                            userVector.push_back(j);
                        }
                    }
                    //对能分配的所有节点进行分配排序。用户节点对应的Node越少越优先分配，目的保证用户节点能分配完。
                    //注意这里的Um->getUserNames()返回是从0开始，但是userVector是从1开始编号，所以要减去1；
                    sort(userVector.begin(),userVector.end(),[=,&Um](int &x,int &y){
                        string name_x = Um->getUserNames()[x-1];
                        string name_y = Um->getUserNames()[y-1];
                        return Um->getUserToNodeNum(name_x) <= Um->getUserToNodeNum(name_y);
                    });

                    for(auto &m : userVector){
                        if(CountJuTemp[m][0] < CountJuTemp[0][colIndex]){
                            //分配完 需求小于总量
                            CountJuTemp[m][colIndex] += CountJuTemp[m][0];
                            CountJuTemp[0][colIndex] -= CountJuTemp[m][0];
                            CountJuTemp[m][0] = 0;
                        } else{
                            //需求大于总量
                            CountJuTemp[m][colIndex] += CountJuTemp[0][colIndex];
                            CountJuTemp[m][0] -= CountJuTemp[0][colIndex];
                            CountJuTemp[0][colIndex] = 0;
                        }
                        if(CountJuTemp[0][colIndex] <=0) break;//没得分配退出。
                    }
                }
            }
        }
        //其余不是fireOut节点进行均分处理
        for (auto &data : CountJuTemp) {
            if (data == CountJuTemp[0] or data[0] == 0) continue;
            //节点更新。
            upgrateUsefulNodeList(CountJuTemp,data);
            int nodeNum = 0,val =0;
            vector<int> nodeIndex{};
            for (int j = 1;j < data.size();j++) {
                if(data[j] >= 0){
                    nodeNum  = nodeNum +1;
                    nodeIndex.push_back(j);
                }
            }
            if(nodeNum == 0) continue;
            val = data[0]/nodeNum;
            if(val == 0) continue;
            averageDis(CountJuTemp,data,val,nodeIndex);
        }

        int Reuslt = 1;
        while (Reuslt != -1)
        {
            AverageChoose(CountJuTemp, Reuslt, 1);
            Reuslt = IsEffect(CountJuTemp);
        }

        //获得分配结果
        for (int m = 1; m < CountJuTemp.size(); m++)
        {
            outfile << Usernames[m - 1] << ":";
            string tempdata;
            for (int j = 1; j < CountJuTemp[m].size(); j++)
                if (CountJuTemp[m][j] > 0)
                {
                    tempdata = tempdata + "<" + Nodenames[j - 1] + "," + to_string(CountJuTemp[m][j]) + ">" + ",";
                }
            if (tempdata.size() > 0)
                tempdata.pop_back();
            outfile << tempdata;
            outfile << endl;
        }
    }
    outfile.close();

    return 0;
}