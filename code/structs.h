#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <QDateTime>
#include <QLabel>
#include <QPropertyAnimation>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <sstream>
using namespace std;

#define INF 0x3f3f3f3f //无穷大

#define STRATEGY_NUM 3 //策略种数

enum Strategy //策略
{
    MIN_COST, //最小费用
    MIN_DUR, //最短时间
    LIM_DUR_MIN_COST //限定时间最小费用
};

//当前城市，用于对A*搜索中结点访问优先次序作判断
extern int curCity;

struct Passenger //旅客结构体
{
    struct Demand //旅客需求结构体
    {
        string name; //旅客姓名
        string startCity, destCity; //出发地与目的地
        vector<string> passCities; //沿途必经城市
        int startTime; //出发时刻（小时）
        QDateTime startDateTime; //出发日期
        Strategy strategy; //旅行策略
        int durLimit = INF; //旅行时间限制
    } demand;

    struct ChangedDemand //需求更改结构体
    {
        bool changeFlag = false; //标记是否更改了需求
        string startCity, destCity; //出发地与目的地
        vector<string> passCities; //沿途必经城市
        int startTime; //出发时刻（小时）
        QDateTime startDateTime; //出发日期
        Strategy strategy; //旅行策略
        int durLimit = INF; //旅行时间限制
    } changedDemand;

    struct State //旅客状态结构体
    {
        int totDur; //旅行总时间
        int totCost; //旅行总费用
        vector<int> revPath; //旅行路径（逆向存储）
        QPropertyAnimation *anime = NULL; //旅行演示动画
        QLabel *label = NULL; //旅客显示图片
        int currentVehicleKind; //当前交通工具种类
        int remainSlot; //剩余时间槽数
        int nextEdgeIndex; //下一条边索引
    } state;

    void changeDemand(void) //需求更改函数
    {
        demand.destCity = changedDemand.destCity;
        demand.startCity = changedDemand.startCity;
        demand.durLimit = changedDemand.durLimit;
        demand.strategy = changedDemand.strategy;
        demand.startDateTime.setDate(changedDemand.startDateTime.date());
        demand.startDateTime.setTime(changedDemand.startDateTime.time());
        demand.startTime = changedDemand.startTime;
        demand.passCities.clear();
        for (int i = 0; i < changedDemand.passCities.size(); i ++)
            demand.passCities.push_back(changedDemand.passCities[i]);
    }

    bool del = false; //标记该旅客是否已被删除
};

extern vector<Passenger> passengers; //旅客数组

struct TrafficMap //交通路线图结构体
{
    #define MAX_EDGE_NUM 10000 //最大边数量
    #define VEHICLE_KIND_NUM 4 //交通工具种数

    enum VehicleKind //交通工具种类
    {
        BUS, TRAIN, PLANE, DEFAULT //巴士，火车，飞机，缺省（即在一个城市候车）
    };

    #define CITY_NUM 10 //城市数量
    #define ONE_HR 1 //一小时
    #define HR_PER_DAY 24 //每天小时数
    #define FREE 0 //免费

    static string cityNoToName[CITY_NUM]; //城市名数组
    map<string, int> cityNameToNo; //城市名转城市编号
    struct Coor //坐标结构体
    {
        int x;
        int y;
    } cityCoor[CITY_NUM] = //城市坐标数组
    {
        {531, 224},
        {626, 151},
        {513, 259},
        {436, 330},
        {578, 349},
        {615, 360},
        {510, 498},
        {498, 424},
        {376, 389},
        {417, 404}
    };

    struct CityTime //城市与时刻结构体（用于表示状态）
    {
        int city, time;
    };

    struct Edge //边结构体
    {
        VehicleKind kind; //交通工具种类
        string vehicleNo; //车次/航班号
        CityTime depart, arrive; //出发城市与时刻，到达城市与时刻
        int dur, cost; //时间与费用
        bool del; //标记该边是否被删除
        bool vis; //标记该边所达状态是否已被访问

        friend ostream& operator <<(ostream& out, const Edge& rhs)
        {
            return out << "/*****************************/\n"
                       << "kind : " << int(rhs.kind) << endl
                       << "vehicleNo : " << rhs.vehicleNo << endl
                       << "departCity : " << cityNoToName[rhs.depart.city] << endl
                       << "departTime : " << rhs.depart.time << endl
                       << "arriveCity : " << cityNoToName[rhs.arrive.city] << endl
                       << "arriveTime : " << rhs.arrive.time << endl
                       << "duration : " << rhs.dur << endl
                       << "cost : " << rhs.cost << endl
                       << "/*****************************/\n";
         }
    } edges[MAX_EDGE_NUM];
    int edgeNum; //边数量

    //交通路线表（邻接表形式），存储从某个城市某个时间出发的所有交通路线
    vector<int> vehicleTable[CITY_NUM][HR_PER_DAY][VEHICLE_KIND_NUM];

        //任意两个城市在任意两个时刻之间的最短费用
    int minCost[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY],
        //任意两个城市在任意两个时刻之间的最短时间
        minDur[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY];
        //任意城市任意时刻出发到另一个城市最小费用的到达时刻
    int minCostArriveTime[CITY_NUM][HR_PER_DAY][CITY_NUM],
        //任意城市任意时刻出发到另一个城市最短时间的到达时刻
        minDurArriveTime[CITY_NUM][HR_PER_DAY][CITY_NUM];
        //任意两个城市在任意两个时刻之间最小费用路径的前驱边（用于路径还原）
    int minCostPrv[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY],
        //任意两个城市在任意两个时刻之间最短时间路径的前驱边（用于路径还原）
        minDurPrv[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY];
        //任意两个城市在任意两个时刻之间最小费用路径的途经城市（位压缩存储）
    int minCostPass[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY],
        //任意两个城市在任意两个时刻之间最短时间路径的途经城市（位压缩存储）
        minDurPass[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY];
        //任意两个城市在任意两个时刻之间最小费用路径的总时间
    int minCostDur[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY],
        //任意两个城市在任意两个时刻之间最短时间路径的总费用
        minDurCost[CITY_NUM][HR_PER_DAY][CITY_NUM][HR_PER_DAY];

    TrafficMap()
    {
        edgeNum = 0;

        #define USE_FLOYD 0

        #if USE_FLOYD

        memset(minCost, 0x3f, sizeof(minCost));
        memset(minDur, 0x3f, sizeof(minDur));
        memset(minCostArriveTime, 0, sizeof(minCostArriveTime));
        memset(minDurArriveTime, 0, sizeof(minDurArriveTime));
        memset(minCostPrv, -1, sizeof(minCostPrv));
        memset(minDurPrv, -1, sizeof(minDurPrv));
        memset(minCostPass, 0, sizeof(minCostPass));
        memset(minDurPass, 0, sizeof(minDurPass));
        memset(minCostDur, 0x3f, sizeof(minCostDur));

        #endif

        for (int cityNo(0); cityNo != CITY_NUM; ++cityNo)
        {
            cityNameToNo[cityNoToName[cityNo]] = cityNo;

            for (int time(0); time != HR_PER_DAY; ++time)
                addEdge(DEFAULT, "WAIT FOR ONE HR",
                        cityNo, time,
                        cityNo, (time + ONE_HR) % HR_PER_DAY,
                        ONE_HR, FREE);
        }

        readFile();

        //clock_t start = clock();

        #if USE_FLOYD

        Floyd();

        #else

        SPFA();

        #endif

        //clock_t stop = clock();

        //cout << stop - start << "ms\n";
    }

    void addEdge(VehicleKind _kind, const string& _vehicleNo,
                 int _departCity, int _departTime,
                 int _arriveCity, int _arriveTime,
                 int _dur, int _cost)
    {
        edges[edgeNum] =
        {
            _kind, _vehicleNo,
            {_departCity, _departTime},
            {_arriveCity, _arriveTime},
            _dur, _cost,
            0, 0
        };

        vehicleTable[_departCity][_departTime][_kind].push_back(edgeNum);

        #if USE_FLOYD

        if ((_cost < minCost[_departCity][_departTime][_arriveCity][_arriveTime])
            || ((_cost == minCost[_departCity][_departTime][_arriveCity][_arriveTime])
                && (_dur < minCostDur[_departCity][_departTime][_arriveCity][_arriveTime])))
        {
            minCost[_departCity][_departTime][_arriveCity][_arriveTime] = _cost;
            minCostPrv[_departCity][_departTime][_arriveCity][_arriveTime] = edgeNum;
            minCostPass[_departCity][_departTime][_arriveCity][_arriveTime] |= (1 << _departCity);
            minCostPass[_departCity][_departTime][_arriveCity][_arriveTime] |= (1 << _arriveCity);
            minCostDur[_departCity][_departTime][_arriveCity][_arriveTime] = _dur;
        }

        if (_dur < minDur[_departCity][_departTime][_arriveCity][_arriveTime])
        {
            minDur[_departCity][_departTime][_arriveCity][_arriveTime] = _dur;
            minDurPrv[_departCity][_departTime][_arriveCity][_arriveTime] = edgeNum;
            minDurPass[_departCity][_departTime][_arriveCity][_arriveTime] |= (1 << _departCity);
            minDurPass[_departCity][_departTime][_arriveCity][_arriveTime] |= (1 << _arriveCity);
        }

        #endif

        ++edgeNum;
    }

    void addEdge(VehicleKind _kind, const string& _vehicleNo,
                 const string& _departCity, int _departTime,
                 const string& _arriveCity, int _arriveTime,
                 int _dur, int _cost)
    {
        addEdge(_kind, _vehicleNo,
                cityNameToNo[_departCity], _departTime,
                cityNameToNo[_arriveCity], _arriveTime,
                _dur, _cost);
    }

    void eraseEdge()
    {

    }

    void readFile()
    {
        QString fileName[VEHICLE_KIND_NUM - 1] =
        {
            ":/bus.txt",
            ":/train.txt",
            ":/plane.txt"
        };

        QFile fin;
        string departCity, arriveCity, vehicleNo,
               departTimeStr, arriveTimeStr;
        int departTime, arriveTime, dur, cost;

        for (int kind(0); kind != VEHICLE_KIND_NUM - 1; ++kind)
        {
            fin.setFileName(fileName[kind]);
            if (!fin.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug("read file error.");
                return;
            }

            QTextStream t(&fin);
            QString qStr;
            while(!t.atEnd())
            {
                qStr = t.readLine();
                string str = qStr.toStdString();
                istringstream iStr(str);
                iStr >> departCity >> arriveCity >> vehicleNo
                     >> departTimeStr >> arriveTimeStr >> cost;

                departTime = atoi(departTimeStr.c_str());

                switch (arriveTimeStr[0])
                {
                    case 's':
                    {
                        arriveTime = atoi(arriveTimeStr.c_str() + 1);
                        dur = arriveTime - departTime + HR_PER_DAY;

                        break;
                    }
                    case 't':
                    {
                        arriveTime = atoi(arriveTimeStr.c_str() + 1);
                        dur = arriveTime - departTime + HR_PER_DAY * 2;

                        break;
                    }
                    default:
                    {
                        arriveTime = atoi(arriveTimeStr.c_str());
                        dur = arriveTime - departTime;

                        break;
                    }
                }

                addEdge(VehicleKind(kind), vehicleNo,
                        departCity, departTime,
                        arriveCity, arriveTime,
                        dur, cost);
            }

            fin.close();
        }

        qDebug("edgeNum = %d\n", edgeNum);
    }

    void writeLog()
    {
        /*
        write changes to log;
        */
    }

    #define FOR(city, time) for (int city(0); city != CITY_NUM; ++city) \
                                for (int time(0); time != HR_PER_DAY; ++time)

    deque<CityTime> q; //用于SPFA最短路算法的队列
    bool vis[CITY_NUM][HR_PER_DAY]; //SPFA最短路算法中标记某个状态（城市，时刻）是否已被访问

    void SPFA()
    {
        memset(minCost, 0x3f, sizeof(minCost));
        memset(minDur, 0x3f, sizeof(minDur));
        memset(minCostArriveTime, 0, sizeof(minCostArriveTime));
        memset(minDurArriveTime, 0, sizeof(minDurArriveTime));
        memset(minCostPrv, -1, sizeof(minCostPrv));
        memset(minDurPrv, -1, sizeof(minDurPrv));
        memset(minCostPass, 0, sizeof(minCostPass));
        memset(minDurPass, 0, sizeof(minDurPass));
        memset(minCostDur, 0x3f, sizeof(minCostDur));
        memset(minDurCost, 0x3f, sizeof(minDurCost));

        FOR(startCity, startTime)
        {
            memset(vis, 0, sizeof(vis));

            minCost[startCity][startTime][startCity][startTime] = 0;
            minCostPrv[startCity][startTime][startCity][startTime] = -1;
            minCostPass[startCity][startTime][startCity][startTime] = (1 << startCity);
            minCostDur[startCity][startTime][startCity][startTime] = 0;
            minCostArriveTime[startCity][startTime][startCity] = startTime;
            minDur[startCity][startTime][startCity][startTime] = 0;
            minDurPrv[startCity][startTime][startCity][startTime] = -1;
            minDurPass[startCity][startTime][startCity][startTime] = (1 << startCity);
            minDurCost[startCity][startTime][startCity][startTime] = 0;
            minDurArriveTime[startCity][startTime][startCity] = startTime;
            q.push_front({startCity, startTime});
            vis[startCity][startTime] = 1;

            while (q.size())
            {
                CityTime cur = q.front();
                q.pop_front();
                vis[cur.city][cur.time] = 0;

                for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                    for (int i : vehicleTable[cur.city][cur.time][kind])
                    {
                        if (edges[i].del)
                            continue;

                        const CityTime& arrive(edges[i].arrive);
                        const int& dur(edges[i].dur);
                        const int& cost(edges[i].cost);
                        int& mcat(minCostArriveTime[startCity][startTime][arrive.city]);
                        int& mdat(minDurArriveTime[startCity][startTime][arrive.city]);

                        if ((minCost[startCity][startTime][arrive.city][arrive.time]
                             > (minCost[startCity][startTime][cur.city][cur.time] + cost))
                            || ((minCost[startCity][startTime][arrive.city][arrive.time]
                                 == (minCost[startCity][startTime][cur.city][cur.time] + cost))
                                && (minCostDur[startCity][startTime][arrive.city][arrive.time]
                                    > (minCostDur[startCity][startTime][cur.city][cur.time] + dur))))
                        {
                            minCost[startCity][startTime][arrive.city][arrive.time]
                                = (minCost[startCity][startTime][cur.city][cur.time] + cost);
                            minCostPrv[startCity][startTime][arrive.city][arrive.time] = i;
                            minCostPass[startCity][startTime][arrive.city][arrive.time]
                                = (minCostPass[startCity][startTime][cur.city][cur.time] | (1 << arrive.city));
                            minCostDur[startCity][startTime][arrive.city][arrive.time]
                                = (minCostDur[startCity][startTime][cur.city][cur.time] + dur);

                            if (!vis[arrive.city][arrive.time] && (vis[arrive.city][arrive.time] = 1))
                            {
                                if (q.size()
                                    && (minCost[startCity][startTime][arrive.city][arrive.time]
                                        < minCost[startCity][startTime][q.front().city][q.front().time]))
                                    q.push_front(arrive);
                                else
                                    q.push_back(arrive);
                            }

                            if ((minCost[startCity][startTime][arrive.city][arrive.time]
                                 < minCost[startCity][startTime][arrive.city][mcat])
                                || ((minCost[startCity][startTime][arrive.city][arrive.time]
                                     == minCost[startCity][startTime][arrive.city][mcat])
                                    && (minCostDur[startCity][startTime][arrive.city][arrive.time]
                                        < minCostDur[startCity][startTime][arrive.city][mcat])))
                                mcat = arrive.time;
                        }

                        if ((minDur[startCity][startTime][arrive.city][arrive.time]
                             > (minDur[startCity][startTime][cur.city][cur.time] + dur))
                            || ((minDur[startCity][startTime][arrive.city][arrive.time]
                                 == (minDur[startCity][startTime][cur.city][cur.time] + dur))
                                && (minDurCost[startCity][startTime][arrive.city][arrive.time]
                                    > (minDurCost[startCity][startTime][cur.city][cur.time] + cost))))
                        {
                            minDur[startCity][startTime][arrive.city][arrive.time]
                                = (minDur[startCity][startTime][cur.city][cur.time] + dur);
                            minDurPrv[startCity][startTime][arrive.city][arrive.time] = i;
                            minDurPass[startCity][startTime][arrive.city][arrive.time]
                                = (minDurPass[startCity][startTime][cur.city][cur.time] | (1 << arrive.city));
                            minDurCost[startCity][startTime][arrive.city][arrive.time]
                                = (minDurCost[startCity][startTime][cur.city][cur.time] + cost);

                            if (!vis[arrive.city][arrive.time] && (vis[arrive.city][arrive.time] = 1))
                            {
                                if (q.size()
                                    && (minDur[startCity][startTime][arrive.city][arrive.time]
                                        < minDur[startCity][startTime][q.front().city][q.front().time]))
                                    q.push_front(arrive);
                                else
                                    q.push_back(arrive);
                            }

                            if ((minDur[startCity][startTime][arrive.city][arrive.time]
                                 < minDur[startCity][startTime][arrive.city][mdat])
                                || ((minDur[startCity][startTime][arrive.city][arrive.time]
                                     == minDur[startCity][startTime][arrive.city][mdat])
                                    && (minDurCost[startCity][startTime][arrive.city][arrive.time]
                                        < minDurCost[startCity][startTime][arrive.city][mdat])))
                                mdat = arrive.time;
                        }
                    }
            }
        }
    }

    void Floyd()
    {
        FOR(passCity, passTime)
            FOR(startCity, startTime)
                FOR(destCity, destTime)
                {
                    int& mcat(minCostArriveTime[startCity][startTime][destCity]);
                    int& mdat(minDurArriveTime[startCity][startTime][destCity]);

                    if ((minCost[startCity][startTime][destCity][destTime]
                         > (minCost[startCity][startTime][passCity][passTime]
                            + minCost[passCity][passTime][destCity][destTime]))
                        || ((minCost[startCity][startTime][destCity][destTime]
                             == (minCost[startCity][startTime][passCity][passTime]
                                 + minCost[passCity][passTime][destCity][destTime]))
                            && (minCostDur[startCity][startTime][destCity][destTime]
                                > (minCostDur[startCity][startTime][passCity][passTime]
                                   + minCostDur[passCity][passTime][destCity][destTime]))))
                    {
                        minCost[startCity][startTime][destCity][destTime]
                            = (minCost[startCity][startTime][passCity][passTime]
                               + minCost[passCity][passTime][destCity][destTime]);
                        minCostPrv[startCity][startTime][destCity][destTime]
                            = minCostPrv[passCity][passTime][destCity][destTime];
                        minCostPass[startCity][startTime][destCity][destTime]
                            = (minCostPass[startCity][startTime][passCity][passTime]
                               | minCostPass[passCity][passTime][destCity][destTime]);
                        minCostDur[startCity][startTime][destCity][destTime]
                            = (minCostDur[startCity][startTime][passCity][passTime]
                               + minCostDur[passCity][passTime][destCity][destTime]);

                        if ((minCost[startCity][startTime][destCity][destTime]
                             < minCost[startCity][startTime][destCity][mcat])
                            || ((minCost[startCity][startTime][destCity][destTime]
                                 == minCost[startCity][startTime][destCity][mcat])
                                && (minCostDur[startCity][startTime][destCity][destTime]
                                    < minCostDur[startCity][startTime][destCity][mcat])))
                            mcat = destTime;
                    }
                    if (minDur[startCity][startTime][destCity][destTime]
                        > (minDur[startCity][startTime][passCity][passTime]
                           + minDur[passCity][passTime][destCity][destTime]))
                    {
                        minDur[startCity][startTime][destCity][destTime]
                            = (minDur[startCity][startTime][passCity][passTime]
                               + minDur[passCity][passTime][destCity][destTime]);
                        minDurPrv[startCity][startTime][destCity][destTime]
                            = minDurPrv[passCity][passTime][destCity][destTime];
                        minDurPass[startCity][startTime][destCity][destTime]
                            = (minDurPass[startCity][startTime][passCity][passTime]
                               | minDurPass[passCity][passTime][destCity][destTime]);
                        if (minDur[startCity][startTime][destCity][destTime]
                            < minDur[startCity][startTime][destCity][mdat])
                            mdat = destTime;
                    }
                }
    }

    struct HeapNode //A*搜索中的估价函数结点
    {
        //该结点在入堆的序号，用于路径还原
        int saveId;
        //该结点所在城市与时刻
        int city, time;
        //A*搜索过程估价函数构成：
        //f = g + h
        //g为起始节点到当前结点路径的权值
        //h为启发式函数，使用预处理的当前结点到目标节点的最小权值
        //f为估价函数，估计在当前节点状态下
        //到目标节点最小路径总权值
        int g, f;
        //gPass为到当前节点已经过的城市（位压缩存储）
        //fPass为若走预估最小路径，到目标节点将途径的城市（位压缩存储）
        int gPass, fPass;
        //到当前节点路径的时间与费用，用于第三种策略
        int dur, cost;

        //在A*搜索中，进行BFS时优先拓展估价函数值小的结点
        //当估价函数值相同时，优先拓展当前已走路径权值小的结点
        //再则优先拓展能离开当前城市的结点（防止在同一个城市死循环）
        //再则优先拓展当前已走路径时间短的结点（用于策略三）
        bool operator >(const HeapNode& rhs) const
        {
            return ((f > rhs.f)
                    || ((f == rhs.f) && (city == curCity))
                    || ((f == rhs.f) && (city != curCity) && (rhs.city != curCity) && (g > rhs.g))
                    || ((f == rhs.f) && (city != curCity) && (rhs.city != curCity) && (g == rhs.g) && (dur > rhs.dur)));
        }
    };

    //用于getStrategy()中A*搜索的最小堆（优先队列）
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode> > heap;

    //保存A*搜索中已入堆结点的前驱信息的结构体，用于路径还原
    struct Save
    {
        //到达该结点的边
        int prvEdge;
        //该结点的前驱结点
        int prv;
    };

    void getStrategy(Passenger& passenger);

    void debug();

    void restoreEdge()
    {
        FOR(city, time)
        {
            for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                for (int i : vehicleTable[city][time][kind])
                    edges[i].vis = 0;
        }
    }
};

extern TrafficMap trafficMap;
extern int selectedPassenger;
#endif // STRUCTS_H

