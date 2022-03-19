import random
import string
import csv
import os
import datetime


class file_path():
    site_bandwidth = '/data/site_bandwidth.csv'
    qos = '/data/qos.csv'
    demand = '/data/demand.csv'

def data_gen(User_count, Edge_count, time_count):
    if User_count > 35 or User_count < 1:
        print("Number of User nodes input out of range")
    if Edge_count > 135 or Edge_count < 1:
        print("Number of Edge nodes input out of range")
    if time_count > 8928 or User_count < 2:
        print("Times input out of range")
    User_list , Edge_list, Time_list = [],[],[]
    alpha = '0123456789'+string.ascii_letters
    for i in range(User_count):
        User_list.append('U' + alpha[i])
    for i in range(Edge_count):
        if i < 62:
            Edge_list.append('E'+alpha[i])
        elif 62 <= i < 124:
            Edge_list.append('F'+alpha[i-62])
        else:
            Edge_list.append('G'+alpha[i-124])
    if not os.path.exists(file_path.site_bandwidth):
        with open(file_path.site_bandwidth, 'w', encoding='UTF-8',newline='') as f1:
            f1_write = csv.writer(f1)
            f1_write.writerow(['site_name','bandwidth'])
    if not os.path.exists(file_path.qos):
        with open(file_path.qos, 'w', encoding='UTF-8',newline='') as f2:
            f2_write = csv.writer(f2)
            f2_write.writerow(['site_name']+User_list)
    if not os.path.exists(file_path.demand):
        with open(file_path.demand, 'w', encoding='UTF-8',newline='') as f3:
            f3_write = csv.writer(f3)
            f3_write.writerow(['mtime']+User_list)
    begin_time = datetime.datetime(2020,10,10,0,0)
    for i in range(time_count):
        new_time = begin_time+datetime.timedelta(minutes=5)
        begin_time = new_time
        Time_list.append(new_time.strftime("%Y-%m-%dT%H:%M"))
    demand_list = []
    for i in range(len(Time_list)):
        demand_temp = []
        demand_temp.append(Time_list[i])
        for j in range(len(User_list)):
            demand_temp.append(random.randint(0,550000))
        demand_list.append(demand_temp)
    if os.path.exists(file_path.demand):
        with open(file_path.demand, 'a', encoding='UTF-8',newline='') as f3:
            f3_write = csv.writer(f3)
            f3_write.writerows(demand_list)
    site_bandwidth_list = []
    for i in range(len(Edge_list)):
        site_temp = []
        site_temp.append(Edge_list[i])
        site_temp.append(random.randint(0,1000000))
        site_bandwidth_list.append(site_temp)
    if os.path.exists(file_path.site_bandwidth):
        with open(file_path.site_bandwidth, 'a', encoding='UTF-8',newline='') as f1:
            f1_write = csv.writer(f1)
            f1_write.writerows(site_bandwidth_list)
    qos_list = []
    for i in range(len(Edge_list)):
        qos_temp = []
        qos_temp.append(Edge_list[i])
        for j in range(len(User_list)):
            qos_temp.append(random.randint(1,1000))
        qos_list.append(qos_temp)
    if os.path.exists(file_path.qos):
        with open(file_path.qos, 'a', encoding='UTF-8',newline='') as f2:
            f2_write = csv.writer(f2)
            f2_write.writerows(qos_list)

if __name__ == '__main__':
    data_gen(35,135,290) #参数1：用户节点数，参数2：边缘节点数：参数3：时间数。
