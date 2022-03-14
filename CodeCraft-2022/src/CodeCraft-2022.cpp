#include <iostream>
#include<fstream>

using namespace std;
string file_root = "/data/";
string file_Demand = "demand.csv";
string file_Bandwidth = "site_bandwidth.csv";
string file_qos = "qos.csv";
string file_config = "config.ini";
int main() {

    //获取客户宽带需求

    ifstream infile(filename);
    while(infile.good()){
    //.csv文件用","作为分隔符
    getline(infile,value,',');
    cout<<value<<endl;


    std::cout << "Hello world!"<<std::endl;
	return 0;
}
