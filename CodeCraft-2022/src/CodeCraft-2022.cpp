#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include"config.h"
using namespace std;
string file_root = "data/";
string file_Demand = "demand.csv";
string file_Bandwidth = "site_bandwidth.csv";
string file_qos = "qos.csv";
string file_config = "config.ini";


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

class DatasStruct {
public:
	vector<string> headName;
	vector<vector< string>> Datas;
};

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
			tempData.Datas.push_back(data);
		//cout << value << endl;
	}
	return tempData;
}


int main() {

	//获取客户宽带需求
	DatasStruct UserWidths = GetData(file_root + file_Demand);

	//获取边缘节点带宽数
	DatasStruct NodeWidths =  GetData(file_root + file_Bandwidth);

	//获取客户与边缘节点网络时延
	DatasStruct PeopleQos = GetData(file_root + file_qos);
	//获取config
	using namespace iniFile_parse;
	Config config;
	int ret = 0;
	ret = parse_ini_file((file_root+file_config).c_str(), &config);
	if (ret != EC_NONE)
	{
		cout<<"ERROR | parse the ini file failed.";
		return EC_ERROR;
	}
	int qos = config["qos_constraint"].as_int();
	return 0;
}
