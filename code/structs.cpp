#include "structs.h"

//当前城市，用于对A*搜索中结点访问优先次序作判断
int curCity;

vector<Passenger> passengers;


string TrafficMap::cityNoToName[CITY_NUM] =
{
    "北京",
    "长春",
    "石家庄",
    "西安",
    "南京",
    "上海",
    "广州",
    "长沙",
    "成都",
    "重庆"
};

int selectedPassenger = -1;

TrafficMap trafficMap;
