#include "structs.h"

void TrafficMap::getStrategy(Passenger& passenger)
{
    const Passenger::Demand& demand(passenger.demand);

    int startCity(cityNameToNo[demand.startCity]),
        startTime(demand.startTime),
        destCity(cityNameToNo[demand.destCity]);
    int passCities(0);

    passCities |= (1 << startCity);
    passCities |= (1 << destCity);
    for (auto passCity : demand.passCities)
        passCities |= (1 << cityNameToNo[passCity]);

    Passenger::State& state(passenger.state);
    state.totCost = 0;
    state.totDur = 0;
    vector<int>& revPath(state.revPath);
    vector<Save> save;
    int saveSize(0);

    revPath.clear();
    while (heap.size())
        heap.pop();

    switch (demand.strategy)
    {
        case MIN_COST:
        {
            int mcat(minCostArriveTime[startCity][startTime][destCity]);

            heap.push({saveSize++,
                       startCity, startTime,
                       0, minCost[startCity][startTime][destCity][mcat],
                       1 << startCity, minCostPass[startCity][startTime][destCity][mcat],
                       0});
            save.push_back({-1, -1});

            while (heap.size())
            {
                HeapNode cur = heap.top();
                heap.pop();

                const int& city(curCity = cur.city);
                const int& time(cur.time);

                if ((cur.fPass & passCities) == passCities)
                {
                    int passCity(destCity),
                        passTime(minCostArriveTime[city][time][destCity]);

                    while (!(passCity == city && passTime == time))
                    {
                        revPath.push_back(minCostPrv[city][time][passCity][passTime]);
                        state.totCost += edges[revPath.back()].cost;
                        state.totDur += edges[revPath.back()].dur;
                        passCity = edges[revPath.back()].depart.city;
                        passTime = edges[revPath.back()].depart.time;
                    }

                    int i(cur.saveId);

                    while (i)
                    {
                        revPath.push_back(save[i].prvEdge);
                        state.totCost += edges[revPath.back()].cost;
                        state.totDur += edges[revPath.back()].dur;
                        i = save[i].prv;
                    }

                    return;
                }

                for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                    for (int i : vehicleTable[city][time][kind])
                    {
                        const Edge& edge(edges[i]);

                        if (edge.vis)
                            continue;

                        mcat = minCostArriveTime[edge.arrive.city][edge.arrive.time][destCity];
                        int h(minCost[edge.arrive.city][edge.arrive.time][destCity][mcat]);
                        int hPass(minCostPass[edge.arrive.city][edge.arrive.time][destCity][mcat]);

                        edges[vehicleTable[city][(time + HR_PER_DAY - 1) % HR_PER_DAY][DEFAULT][0]].vis = 1;

                        try
                        {
                            heap.push({saveSize++,
                                       edge.arrive.city, edge.arrive.time,
                                       cur.g + edge.cost, cur.g + edge.cost + h,
                                       cur.gPass | (1 << edge.arrive.city), cur.gPass | hPass,
                                       cur.dur + edge.dur});
                            save.push_back({i, cur.saveId});
                        }
                        catch (bad_alloc)
                        {
                            state.totCost = INF;
                            state.totDur = -1;
                            return;
                        }
                    }
            }

            break;
        }
        case MIN_DUR:
        {
            int mdat(minDurArriveTime[startCity][startTime][destCity]);

            heap.push({saveSize++,
                       startCity, startTime,
                       0, minDur[startCity][startTime][destCity][mdat],
                       1 << startCity, minDurPass[startCity][startTime][destCity][mdat],
                       0});
            save.push_back({-1, -1});

            while (heap.size())
            {
                HeapNode cur = heap.top();
                heap.pop();

                const int& city(curCity = cur.city);
                const int& time(cur.time);

                if ((cur.fPass & passCities) == passCities)
                {
                    int passCity(destCity),
                        passTime(minDurArriveTime[city][time][destCity]);

                    while (!(passCity == city && passTime == time))
                    {
                        revPath.push_back(minDurPrv[city][time][passCity][passTime]);
                        state.totCost += edges[revPath.back()].cost;
                        state.totDur += edges[revPath.back()].dur;
                        passCity = edges[revPath.back()].depart.city;
                        passTime = edges[revPath.back()].depart.time;
                    }

                    int i(cur.saveId);

                    while (i)
                    {
                        revPath.push_back(save[i].prvEdge);
                        state.totCost += edges[revPath.back()].cost;
                        state.totDur += edges[revPath.back()].dur;
                        i = save[i].prv;
                    }

                    return;
                }

                for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                    for (int i : vehicleTable[city][time][kind])
                    {
                        const Edge& edge(edges[i]);

                        if (edge.vis)
                            continue;

                        mdat = minDurArriveTime[edge.arrive.city][edge.arrive.time][destCity];
                        int h(minDur[edge.arrive.city][edge.arrive.time][destCity][mdat]);
                        int hPass(minDurPass[edge.arrive.city][edge.arrive.time][destCity][mdat]);

                        edges[vehicleTable[city][(time + HR_PER_DAY - 1) % HR_PER_DAY][DEFAULT][0]].vis = 1;

                        try
                        {
                            heap.push({saveSize++,
                                       edge.arrive.city, edge.arrive.time,
                                       cur.g + edge.dur, cur.g + edge.dur + h,
                                       cur.gPass | (1 << edge.arrive.city), cur.gPass | hPass,
                                       cur.dur + edge.dur});
                            save.push_back({i, cur.saveId});
                        }
                        catch (bad_alloc)
                        {
                            state.totCost = INF;
                            state.totDur = -1;
                            return;
                        }
                    }
            }

            break;
        }
        case LIM_DUR_MIN_COST:
        {
            //第三种策略的A*搜索中，
            //当最小费用路径的总时间比旅行时间限制大
            //且差达到此阈值时，采用以时间为主的搜索方式
            #define DUR_DIFF_THRESHOLD 20
            //第三种策略的A*搜索中，
            //当必须途经城市数量不超过此阈值时
            //采用以时间为主的搜索方式
            #define CITY_NUM_THRESHOLD 3

            #define DUR_LIM_THRESHOLD 40

            const int& durLimit(demand.durLimit);

            if (demand.passCities.size() <= CITY_NUM_THRESHOLD
                && durLimit <= DUR_LIM_THRESHOLD)
                goto BASED_ON_DUR;

            {
                int _minDur = INF;
                int mcat(minCostArriveTime[startCity][startTime][destCity]);

                heap.push({saveSize++,
                           startCity, startTime,
                           0, minCost[startCity][startTime][destCity][mcat],
                           1 << startCity, minCostPass[startCity][startTime][destCity][mcat],
                           0});
                save.push_back({-1, -1});

                while (heap.size())
                {
                    HeapNode cur = heap.top();
                    heap.pop();

                    const int& city(curCity = cur.city);
                    const int& time(cur.time);

                    mcat = minCostArriveTime[city][time][destCity];

                    if ((cur.fPass & passCities) == passCities)
                    {

                        _minDur = min(_minDur, cur.dur + minCostDur[city][time][destCity][mcat]);

                        if (_minDur - durLimit >= DUR_DIFF_THRESHOLD)
                        {
                            goto BASED_ON_DUR;
                        }

                        if (cur.dur + minCostDur[city][time][destCity][mcat] <= durLimit)
                        {
                            int passCity(destCity),
                                passTime(mcat);

                            while (!(passCity == city && passTime == time))
                            {
                                revPath.push_back(minCostPrv[city][time][passCity][passTime]);
                                state.totCost += edges[revPath.back()].cost;
                                state.totDur += edges[revPath.back()].dur;
                                passCity = edges[revPath.back()].depart.city;
                                passTime = edges[revPath.back()].depart.time;
                            }

                            int i(cur.saveId);

                            while (i)
                            {
                                revPath.push_back(save[i].prvEdge);
                                state.totCost += edges[revPath.back()].cost;
                                state.totDur += edges[revPath.back()].dur;
                                i = save[i].prv;
                            }

                            return;
                        }
                    }

                    for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                        for (int i : vehicleTable[city][time][kind])
                        {
                            const Edge& edge(edges[i]);

                            if (edge.vis)
                                continue;

                            mcat = minCostArriveTime[edge.arrive.city][edge.arrive.time][destCity];
                            int h(minCost[edge.arrive.city][edge.arrive.time][destCity][mcat]);
                            int hPass(minCostPass[edge.arrive.city][edge.arrive.time][destCity][mcat]);

                            edges[vehicleTable[city][(time + HR_PER_DAY - 1) % HR_PER_DAY][DEFAULT][0]].vis = 1;

                            try
                            {
                                heap.push({saveSize++,
                                           edge.arrive.city, edge.arrive.time,
                                           cur.g + edge.cost, cur.g + edge.cost + h,
                                           cur.gPass | (1 << edge.arrive.city), cur.gPass | hPass,
                                           cur.dur + edge.dur});
                                save.push_back({i, cur.saveId});
                            }
                            catch (bad_alloc)
                            {
                                state.totCost = INF;
                                state.totDur = -1;
                                return;
                            }
                        }
                }

                break;
            }

            BASED_ON_DUR:
            {
                while (heap.size())
                    heap.pop();
                save.clear();
                saveSize = 0;

                int _minCost = INF;
                int _minCostId;
                int mdat(minDurArriveTime[startCity][startTime][destCity]);

                heap.push({saveSize++,
                           startCity, startTime,
                           0, minDur[startCity][startTime][destCity][mdat],
                           1 << startCity, minDurPass[startCity][startTime][destCity][mdat],
                           0, 0});
                save.push_back({-1, -1});
                restoreEdge();

                while (heap.size())
                {
                    HeapNode cur = heap.top();
                    heap.pop();

                    const int& city(curCity = cur.city);
                    const int& time(cur.time);

                    if (cur.f > durLimit)
                    {
                        if (_minCost == INF)
                        {
                            cout << "strategy not found\n";
                            state.totCost = INF;
                            state.totDur = -1;
                            return;
                        }

                        int mcCity, mcTime;

                        if (!~save[_minCostId].prvEdge)
                        {
                            mcCity = startCity;
                            mcTime = startTime;
                        }
                        else
                        {
                            mcCity = edges[save[_minCostId].prvEdge].arrive.city;
                            mcTime = edges[save[_minCostId].prvEdge].arrive.time;
                        }

                        int passCity(destCity),
                            passTime(minDurArriveTime[mcCity][mcTime][destCity]);

                        while (!(passCity == mcCity && passTime == mcTime))
                        {
                            revPath.push_back(minDurPrv[mcCity][mcTime][passCity][passTime]);
                            state.totCost += edges[revPath.back()].cost;
                            state.totDur += edges[revPath.back()].dur;
                            passCity = edges[revPath.back()].depart.city;
                            passTime = edges[revPath.back()].depart.time;
                        }

                        int i(_minCostId);

                        while (i)
                        {
                            revPath.push_back(save[i].prvEdge);
                            state.totCost += edges[revPath.back()].cost;
                            state.totDur += edges[revPath.back()].dur;
                            i = save[i].prv;
                        }

                        return;
                    }

                    mdat = minDurArriveTime[city][time][destCity];

                    if ((cur.fPass & passCities) == passCities
                        && (cur.cost + minDurCost[city][time][destCity][mdat] < _minCost))
                    {
                        _minCost = cur.cost + minDurCost[city][time][destCity][mdat];
                        _minCostId = cur.saveId;
                    }

                    for (int kind(0); kind != VEHICLE_KIND_NUM; ++kind)
                        for (int i : vehicleTable[city][time][kind])
                        {
                            const Edge& edge(edges[i]);

                            if (edge.vis)
                                continue;

                            mdat = minDurArriveTime[edge.arrive.city][edge.arrive.time][destCity];
                            int h(minDur[edge.arrive.city][edge.arrive.time][destCity][mdat]);
                            int hPass(minDurPass[edge.arrive.city][edge.arrive.time][destCity][mdat]);

                            edges[vehicleTable[city][(time + HR_PER_DAY - 1) % HR_PER_DAY][DEFAULT][0]].vis = 1;

                            try
                            {
                                heap.push({saveSize++,
                                           edge.arrive.city, edge.arrive.time,
                                           cur.g + edge.dur, cur.g + edge.dur + h,
                                           cur.gPass | (1 << edge.arrive.city), cur.gPass | hPass,
                                           cur.dur + edge.dur, cur.cost + edge.cost});
                                save.push_back({i, cur.saveId});
                            }
                            catch (bad_alloc)
                            {
                                state.totCost = INF;
                                state.totDur = -1;
                                return;
                            }
                        }
                }

                break;
            }
        }
    }
}
