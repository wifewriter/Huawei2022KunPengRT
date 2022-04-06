#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<set>
#include<algorithm>
#include<unordered_map>
using namespace std;

//һ�����ݸ�ʽ����
class DatasStruct {
public:
	vector<string> headName;
	vector<vector< string>> Datas;
};

class UserManage
{
private:
	//�����û���
	vector<string> Usernames;
    //��ͬʱ���û��������
    vector<unordered_map<string, int>> WidthNeed;
    vector<unordered_map<string,int>> widthNeedCopy;
    //ÿ���û�����Ӧ�Ŀ�ʹ�õĽڵ���Ŀ
    unordered_map<string,int> userToNodeNum;
public:
    vector<string>& Get_usersnames() { return Usernames; };
	void SetWidth(int time, int width, const string& name) { WidthNeed.resize(time + 1); WidthNeed[time][name] = width; }
	void SetWidthCopy(int time, int width, const string& name) { widthNeedCopy.resize(time + 1); widthNeedCopy[time][name] = width; }
	int GetWidth(int time, const string& name) {
		return WidthNeed[time][name];
	}
	void PushName(const string& name) { Usernames.push_back(name); };
    vector<unordered_map<string, int>>& getWidthNeed(){
        return this->WidthNeed;
    }
    vector<unordered_map<string, int>>& getWidthNeedCopy(){
        return this->widthNeedCopy;
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
	//�����û��ڵ�
	vector<string> Nodenames;
	//�ڵ����±�����
	unordered_map<string,int> nameToIndexMap;
	//�洢Ŀ��ڵ��µĿ����û�����
	unordered_map<string, vector<bool>> UsefulUser_flag;
	//�洢node�Ĵ���
	unordered_map<string, int> Node_Width;
	//�洢�ڵ���غ����� ǰ5%�� �� ʱ��
	unordered_map<string ,vector<vector<int>>> payload_List_map;
	//�洢�ڵ��Ӧ�����fireOut����
	unordered_map<string,int> Node_Times_Map;
    //�洢�Ѿ���������Node�ڵ�
    vector<string> springNodeList;
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
    void setNodeTimesMap(const string &nodeName,int &times){
        this->Node_Times_Map[nodeName] = times;
    }


    //��ȡһ��ȫ������Ľڵ�
    string getOneFireOutNode(int timeIndex,vector<unordered_map<string,int>>& widthNeed,vector<string>& userNames,string &preNode){
        if(preNode.empty()){
            this->springNodeList.clear();
            //�գ�ÿ��ʱ�̿�ʼʱ�ĳ�ʼ�ڵ�
            for (auto &i : this->payload_List_map) {
                for (auto &j:i.second) {
                    if(j[0] <= 0) break;
                    if(j[1] == timeIndex){
                        //�ҵ�Node ,����������Ľڵ㵱�У�ͬʱ�����������
                        this->springNodeList.push_back(i.first);
                        preNode = i.first;
                        this->Node_Times_Map[i.first] -=1;
                        return i.first;
                    }
                }
            }
        } else{
           //�ȸ���payload_List_map�� ��ȡ�ڵ�,
           //��һ�������滻��widthNeed����
            for (auto &i :this->UsefulUser_flag) {
                vector<vector<int>> totalPayload;
                for (int k = timeIndex;k<widthNeed.size();k++) {
                    int perNodeSum = 0;
                    for (int j = 0; j <widthNeed[k].size() ; ++j) {
                        if(i.second[j] == 1){
                            perNodeSum += widthNeed[k][userNames[j]];
                        }
                    }
                    totalPayload.push_back(vector<int>{perNodeSum,k});
                }
                sort(totalPayload.begin(),totalPayload.end(),greater<vector<int>>());
                //��ȡ��ʣ�µĴ���
                int interval = this->Node_Times_Map[i.first];

                //һ���ڵ�
                if(interval <= 0){
                    this->payload_List_map[i.first] = vector<vector<int >>{{0}};
                } else{
                    if(interval >= totalPayload.size()){
                        interval = totalPayload.size();
                    }if(!totalPayload.empty()){
                        this->payload_List_map[i.first] = vector<vector<int >>(totalPayload.begin(),totalPayload.begin()+interval-1);
                    } else{
                        this->payload_List_map[i.first] = vector<vector<int >>{{0}};
                    }
                }
            }
            //�ڶ��������º��ٻ�ȡ��ǰʱ��Ӧ������Ľڵ�
            for (auto &i : this->payload_List_map) {
                for (auto &j:i.second) {
                    if(j[0] <= 0) break;
                    auto k = find(this->springNodeList.begin(),this->springNodeList.end(),i.first) = this->springNodeList.end();
                    //�����δ�ҵ���
                    if(j[1] == timeIndex and k == this->springNodeList.end()){
                        //�ҵ�Node ,����������Ľڵ㵱�У�ͬʱ�����������
                        this->springNodeList.push_back(i.first);
                        preNode = i.first;
                        this->Node_Times_Map[i.first] -= 1;
                        return i.first;
                    }
                }
            }
        }
        //û���ҵ��κνڵ�
        return "";
    }

};
//��ȡǰ5%��
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
        //һ���ڵ�
        this->payload_List_map[i.first] = vector<vector<int >>(totalPayload.begin(),totalPayload.begin()+max*0.05);
    }
}
//TODO:�������ھ���ǰ��һ��ͷ�������±�������Ҫ+1��
void NodeManage::setNameToIndexMap() {
    for (int i = 0; i < Nodenames.size(); ++i) {
        this->nameToIndexMap[Nodenames[i]] = i+1;
    }
}
//�ַ����и��
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
//ȥ�����з�
void strim(string& str)
{
	int Pos = str.size() - 1;
	if (str[Pos] == '\n' || str[Pos] == '\r')
	{
		str.erase(str.begin() + Pos);
	}
}
//��ȡ�ṹ������
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
//��ȡini�ļ��е�����
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
//���һ���û��ķ���
void NodeAssign(vector<vector<int>>& allc, int user, int node)
{
	for (int j = node; j < allc[user].size(); j++)
	{
		//������ܷ���
		if (allc[user][j] == -1)
			continue;
		else
		{
			//�ڵ�ɷ���Ϊ0
			if (allc[0][j] == 0)
				continue;
			//�û�����С�ڵ��ڿɷ���
			if (allc[user][0] <= allc[0][j])
			{
				allc[user][j] += allc[user][0];
				allc[0][j] -= allc[user][0];
				allc[user][0] -= allc[user][0];
			}
			//�û�������ڿɷ���
			else
			{
				//�÷������д���
				//����������λ��
				allc[user][j] += allc[0][j];
				int temp = allc[user][0];
				//�����û�����
				allc[user][0] -= allc[0][j];
				//���½ڵ����
				allc[0][j] -= allc[0][j];
			}
		}
	}
}
//���·���
void Reset(vector<vector<int>>& allc, int user, int node)
{
	//���ٷ���
	int Rest_width = allc[user][0];
	for (int i = 1; i < user; i++)
	{
		//����ɷ���
		if (allc[i][node] != -1)
		{
			//����ѷ����������
			if (allc[i][node] > Rest_width)
			{
				allc[i][node] -= Rest_width;
				allc[i][0] += Rest_width;
				Rest_width -= Rest_width;
			}
			//����ѷ���С������
			else
			{
				Rest_width -= allc[i][node];
				allc[i][node] -= allc[i][node];
			}
		}
	}
}
//�������� ̰���㷨 user��Ŀ���û����䣬node ��Ŀ��ڵ����
void AverageChoose(vector<vector<int>>& allc, int user, int node)
{
	if (user < allc.size())
		NodeAssign(allc, user, node);
	else
		return;
	//���û�з�����
	if (allc[user][0] != 0)
	{
		for (int j = node; j < allc[user].size(); j++)
		{
			if (allc[user][j] > 0)
			{
				//Reset�����·���
				Reset(allc, user, j);
				//�Ը��û�����
				AverageChoose(allc, user, j);
				if (allc[user][0] == 0)
					break;
			}
		}
		return;
	}
	//�����������һ��
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
    //������һ����ͷ
    for (int i = 1; i < data.size() ; ++i) {
        if(CountJuTemp[0][i] <= 0 and data[i] == 0) data[i] = -1;//TODO:����data[i]��������˵Ļ��Ͳ����á�
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
            //TODO:���ƽ�����䵽���û�з����꣬����û�д���
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
	//��ȡ��Ե�ڵ������
	DatasStruct NodeWidths = GetData( "/data/site_bandwidth.csv");
	//��ȡ�ͻ����Ե�ڵ�����ʱ��
	DatasStruct PeopleQos = GetData("/data/qos.csv");

	//��ȡconfig
	int qos = ReadQos("/data/config.ini");

	auto* Um = new UserManage;
	auto* Nm = new NodeManage;

	//�����Ҫ���ȵ����ʱ����
	int Max_times = UserWidths.Datas.size();
	int numMaxTimes = Max_times*0.04;
	//��ʼ���û� 
	for (int i = 1; i < PeopleQos.headName.size(); i++)
	{

		string username = PeopleQos.headName[i];
		Um->PushName(username);
	}
	//��ʼ��node
	for (auto & Data : NodeWidths.Datas)
	{
		string Nodename = Data[0];
		Nm->PushName(Nodename);
		int NodeWidth = stoi(Data[1].c_str());
		Nm->SetWidth(Nodename, NodeWidth);
	}
	//��ʼ��Node�����û�
	for (int i = 0; i < PeopleQos.Datas.size(); i++)
	{
		for (int j = 1; j < PeopleQos.Datas[i].size(); j++)
		{
			//�ӳ�С����ֵ
			int this_Qos = stoi(PeopleQos.Datas[i][j]);
			string Nodename = PeopleQos.Datas[i][0];
            Nm->AddNode_Usefuluser(Nodename, this_Qos < qos);
		}
		Nm->setNodeTimesMap(PeopleQos.Datas[i][0],numMaxTimes);
	}
	//��ʼ�û���ͬʱ�����軯����
	for (int time = 0; time < UserWidths.Datas.size(); time++)
	{
		for (int j = 1; j < UserWidths.Datas[time].size(); j++)
		{
			string Username = UserWidths.headName[j];
			//��ø�ʱ�����û�
			int width = stoi(UserWidths.Datas[time][j].c_str());
			//���ø�ʱ�����û��������
			Um->SetWidth(time, width, Username);
			Um->SetWidthCopy(time,width,Username);
		}
	}
	//����ÿ���ڵ�ķ������
    Nm->GetPre5percentPayloadList(Um->getWidthNeed(),Um->getUserNames(),Max_times);
    Nm->setNameToIndexMap();

    //�������---begin
    //��ʼ���������
    vector<string>Usernames = Um->Get_usersnames();
    vector<string>Nodenames = Nm->Get_Nodenames();
    vector<vector<int>> CountJu(Usernames.size() + 1, vector<int>(Nodenames.size() + 1));
    //��ͷ �û��������
    //��ͷ Node��ӵ�еĴ���

    //��ʼ�����������ֵ ���ܷ���Ϊ-1
    for (int i = 0; i < Nodenames.size(); i++)
    {
        vector<bool> flags = Nm->Get_UsefulUser(Nodenames[i]);
        for (int j = 0; j < flags.size(); j++)
        {
            if (flags[j]){
                CountJu[j+1][i+1] = 0;
                Um->setUserToNodeNum(Um->getUserNames()[j]);
            }
            else //���ܷ���ڵ���Ϊ-1
                CountJu[j+1][i+1] = -1;
        }
    }

    //ʱ��Ĵ�ѭ��
    for (int i = 0; i < Max_times; i++)
    {
        vector<vector<int>> CountJuTemp = CountJu;
        //��ȡ��ǰʱ���µ�ӳ��

        for (int k = 1; k < Usernames.size() + 1; k++)
        {
            //��ͷ
            CountJuTemp[k][0] = Um->GetWidth(i, Usernames[k - 1]);
        }
        for (int j = 1; j < Nodenames.size() + 1; j++)
        {
            //��ͷ
            CountJuTemp[0][j] = Nm->GetWidth(Nodenames[j - 1]);
        }
        //�жϵ�ǰʱ�����Щnode��Ҫ��ȫ�������������node��������
        string preNode;
        auto l = Nm->getOneFireOutNode(i, Um->getWidthNeedCopy(), Um->getUserNames(),preNode);
        for (; !l.empty();l = Nm->getOneFireOutNode(i, Um->getWidthNeedCopy(), Um->getUserNames(),preNode)) {

            //��ǰ�ڵ�ΪfireOut�ڵ㡢������ȫ�������غ���ȥ��
            int colIndex = Nm->getNameToIndexMap(l); //��ȡ���±�
            //��ȡ�ܷ����û��ڵ�����о����±�
            vector<int> userVector{};
            for (int j = 0; j < CountJuTemp.size(); ++j) {
                if (CountJuTemp[j][colIndex] == 0) {
                    userVector.push_back(j);
                }
            }
            //���ܷ�������нڵ���з��������û��ڵ��Ӧ��NodeԽ��Խ���ȷ��䣬Ŀ�ı�֤�û��ڵ��ܷ����ꡣ
            //ע�������Um->getUserNames()�����Ǵ�0��ʼ������userVector�Ǵ�1��ʼ��ţ�����Ҫ��ȥ1��
            sort(userVector.begin(), userVector.end(), [=, &Um](int &x, int &y) {
                string name_x = Um->getUserNames()[x - 1];
                string name_y = Um->getUserNames()[y - 1];
                return Um->getUserToNodeNum(name_x) <= Um->getUserToNodeNum(name_y);
            });

            for (auto &m : userVector) {
                if (CountJuTemp[m][0] < CountJuTemp[0][colIndex]) {
                    //������ ����С������
                    CountJuTemp[m][colIndex] += CountJuTemp[m][0];
                    CountJuTemp[0][colIndex] -= CountJuTemp[m][0];
                    CountJuTemp[m][0] = 0;
                } else {
                    //�����������
                    CountJuTemp[m][colIndex] += CountJuTemp[0][colIndex];
                    CountJuTemp[m][0] -= CountJuTemp[0][colIndex];
                    CountJuTemp[0][colIndex] = 0;
                }
                //���·������
                Um->getWidthNeedCopy()[i][Um->getUserNames()[m-1]] -= CountJuTemp[m][colIndex];
                if (CountJuTemp[0][colIndex] <= 0) break;//û�÷����˳���
            }
        }
        //���಻��fireOut�ڵ���о��ִ���

        for (auto &data : CountJuTemp) {
            if (data == CountJuTemp[0] or data[0] == 0) continue;
            //�ڵ���¡�
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

        //��÷�����
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
//        Um->getWidthNeedCopy()[i] = {pair<string,int>("",0)};
//        cout<<"num:"<<i<<endl;
    }
    outfile.close();

    return 0;
}