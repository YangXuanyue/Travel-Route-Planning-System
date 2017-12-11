#include <algorithm>
#include <QAccessible>
#include "mainWindow.h"
#include "mainWidget.h"
#include "addDialog.h"
#include "changeDialog.h"

#define TIMER 1000
#define xOffset -13
#define yOffset 55
int MainWindow::pause(void)
{
    for (int i = 0 ;i < passengers.size(); i ++)
    {
        Passenger& psg = passengers[i];
        if(psg.state.anime != NULL && psg.state.anime->state() == QPropertyAnimation::Running)
            psg.state.anime->pause();
    }
    int remainingTime = timer->remainingTime();
    qDebug("remainingTime = %d", remainingTime);
    timer->stop();
    return remainingTime;

}

void MainWindow::start(int remainingTime)
{
    timer->start(remainingTime);
    for (int i = 0 ;i < passengers.size(); i ++)
    {
        Passenger& psg = passengers[i];

        if(psg.state.anime != NULL)
            psg.state.anime->resume();
    }
}

void MainWindow::createActions(void)
{
    qDebug("enter createActions");
    addAction = new QAction("添加旅客", this);
//    addAction->setIcon(QIcon(":/images/add.png"));
    addAction->setShortcut(tr("Ctrl+A"));
    addAction->setStatusTip("点击添加一个新的旅客");
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    changeAction = new QAction("更改旅客", this);
    changeAction->setShortcut(tr("Ctrl+B"));
    changeAction->setStatusTip("点击更改当前旅客的需求");
    connect(changeAction, SIGNAL(triggered()), this, SLOT(change()));
    changeAction->setEnabled(false);

    deleteAction = new QAction("删除旅客", this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip("点击删除当前旅客");
    connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteSlot()));
    deleteAction->setEnabled(false);

    openLogAction = new QAction("打开日志", this);
    openLogAction->setShortcut(tr("Ctrl+L"));
    connect(openLogAction, SIGNAL(triggered(bool)), this, SLOT(openLog()));
    aboutAction = new QAction("关于", this);
    aboutAction->setShortcut(tr("F1"));
    aboutAction->setStatusTip("关于本软件");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));


    aboutQtAction = new QAction("关于Qt", this);
    aboutQtAction->setShortcut(tr("F2"));
    aboutAction->setStatusTip("关于Qt");
    connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(QMessageBox::aboutQt()));
    qDebug("leave createActions");
}

void MainWindow::createMenus(void)
{
    qDebug("enter createMenus");
    manageMenu = menuBar()->addMenu("旅客管理");

    manageMenu->addAction(addAction);
    manageMenu->addAction(changeAction);
    manageMenu->addAction(deleteAction);

    menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu("帮助");
    helpMenu->addAction(openLogAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    qDebug("leave createMenus");
    return;
}

void MainWindow::openLog(void)
{
/*    QString path=QDir::currentPath();
    path.replace("/","\\");
    QProcess::startDetached("explorer "+path);*/

    int remainingTime = pause();
    fLog.close();
    system("travelAgent.log");
    fLog.open("travelAgent.log", ios_base::app);
    start(remainingTime);
    return;
}

void MainWindow::about(void)
{
    QMessageBox::about(this, "关于本软件", "<h2>旅行线路规划</h2>"
                                      "<p>Copyright ? 2016 YJBBRQW Inc."
                                      "<p>本软件通过现代化信息手段，建设动态旅行规划系统，对用户需求进行分析跟进。"
                                      "基于互联网掌握全面的交通项目信息，提供用户私人旅行路线建议，"
                                      "以方便社会人查询交通信息、制定出行计划，促进交通事业的管理和发展。"
                                      "根据旅客要求为其设计旅行线路。");
}

void MainWindow::add(void)
{
    qDebug("enter add");
    AddDialog addDialog(this);
    int remainingTime = pause();
    while(addDialog.exec())
    {
        Passenger newPassenger;
        addDialog.getData(newPassenger);
        if (newPassenger.demand.name.empty())
            QMessageBox::information(this, "添加旅客错误", "请输入旅客姓名。", QMessageBox::Ok);
        else if (newPassenger.demand.startCity == newPassenger.demand.destCity)
            QMessageBox::information(this, "添加旅客错误", "出发城市和目的城市不能相同。", QMessageBox::Ok);
        else if(newPassenger.demand.durLimit == 0)
            QMessageBox::information(this, "添加旅客错误", "时间限制不能为空。", QMessageBox::Ok);
        else
        {
           passengers.push_back(newPassenger);
            QMessageBox::information(this, "添加旅客成功", "添加旅客成功。", QMessageBox::Ok);
            vector<Passenger>::iterator it = passengers.end() - 1;

            trafficMap.getStrategy(passengers.at(passengers.size() - 1));
            trafficMap.restoreEdge();
            Passenger& psg = passengers.at(passengers.size() - 1);

            for (auto it = psg.state.revPath.begin(); it != psg.state.revPath.end(); )
            {
                if ((*it) < 0 || (*it) >= trafficMap.edgeNum)
                    it = psg.state.revPath.erase(it);
                else
                    ++it;
            }

            if (psg.state.totDur == -1)
            {
                QMessageBox::information(this, "该时间限制内无合法路径", "请放宽时间限制。");
                deletePassenger(passengers.size() - 1);
                break;
            }
            psg.state.label = new QLabel("fuck");
            psg.state.label->setPixmap(QPixmap(":/images/stay.png"));
            psg.state.label->setParent(this);
            psg.state.label->hide();
            int x = trafficMap.cityCoor[trafficMap.cityNameToNo[psg.demand.startCity]].x;
            int y = trafficMap.cityCoor[trafficMap.cityNameToNo[psg.demand.startCity]].y;
            psg.state.label->setGeometry(x + xOffset, y + yOffset, 42, 42);
            psg.state.remainSlot = dateTime->secsTo(psg.demand.startDateTime) / 3600 - 1;
            psg.state.nextEdgeIndex = psg.state.revPath.size() - 1;
            psg.state.anime = NULL;
            qDebug("starttime = %s", psg.demand.startDateTime.toString().toStdString().c_str());
            qDebug("remainSlot = %d", psg.state.remainSlot);
            emit updatePassengerListWidget();
            fLog << dateTime->toString("yyyy年MM月dd日hh时：").toStdString() << "添加旅客" << psg.demand.name << "成功。"
                 << "出发城市:" << psg.demand.startCity << "，目的城市：" << psg.demand.destCity
                 << "，花费时间：" << psg.state.totDur << "，花费金钱：" << psg.state.totCost <<endl;

            break;
        }
    }
    start(remainingTime);
    qDebug("leave add");
    return;
}

void MainWindow::change(void)
{
    qDebug("enter change");
    ChangeDialog changeDialog(this);

    int remainingTime = pause();

    while(changeDialog.exec())
    {
        qDebug("fff");
        Passenger& newPassenger(passengers[selectedPassenger]);
        changeDialog.getData(newPassenger);
        if (newPassenger.changedDemand.startCity == newPassenger.changedDemand.destCity)
            QMessageBox::information(this, "更改旅客错误", "出发城市和目的城市不能相同。", QMessageBox::Ok);
        else if(newPassenger.changedDemand.durLimit == 0)
            QMessageBox::information(this, "更改旅客错误", "时间限制不能为空。", QMessageBox::Ok);
        else
        {
            QMessageBox::information(this, "更改旅客成功", "更改旅客成功。", QMessageBox::Ok);
            newPassenger.changedDemand.changeFlag = true;
            fLog << dateTime->toString("yyyy年MM月dd日hh时：").toStdString() << "更改旅客" << newPassenger.demand.name << "成功。"
                 << "出发城市:" << newPassenger.demand.startCity << "，目的城市：" << newPassenger.demand.destCity << endl;

            break;
        }
    }

    start(remainingTime);
    qDebug("leave change");
    return;
}

void MainWindow::deletePassenger(int selectedPassenger)
{


    qDebug("enter deletePassenger");
    if (!(selectedPassenger >= 0 && selectedPassenger < passengers.size()))
        return;
    fLog << dateTime->toString("yyyy年MM月dd日hh时：").toStdString() << "删除旅客" << passengers[selectedPassenger].demand.name << "成功。\n";
    passengers[selectedPassenger].del = true;
    for (auto it = passengers.begin(); it != passengers.end();)
    {
        if (it->del)
        {
            if (it->state.anime)
                it->state.anime->stop();
            if (it->state.label)
            it->state.label->~QLabel();
                passengers.erase(it);
        }
        else
            it++;
    }

    emit updatePassengerListWidget();
    qDebug("leave deletePassenger");
}

void MainWindow::deleteSlot()
{
    deletePassenger(selectedPassenger);
    return;
}

void MainWindow::processingTimeout(void)
{

    qDebug("enter processingTimeout");
    dateTime = new QDateTime(dateTime->addSecs(3600));
    emit updateTime();
    qDebug("%s", dateTime->toString("yyyy-MM-dd-hh").toStdString().c_str());

    timer->stop();
    timer->start(TIMER);

    for (int i = 0; i < passengers.size(); i ++)
    {
        if (passengers[i].del)
            continue;
        Passenger& psg = passengers[i];
        if (psg.changedDemand.changeFlag == true && psg.state.remainSlot == 0)
        {
            qDebug("change change");

            psg.changeDemand();
            qDebug("starttime = %d", psg.changedDemand.startTime);
            qDebug("startcity = %s", psg.changedDemand.startCity.c_str());
            qDebug("destcity = %s", psg.changedDemand.destCity.c_str());
            qDebug("strategy = %d", psg.changedDemand.strategy);
            qDebug("durlim = %d", psg.changedDemand.durLimit);
            qDebug("starttime = %d", psg.demand.startTime);
            qDebug("startcity = %s", psg.demand.startCity.c_str());
            qDebug("destcity = %s", psg.demand.destCity.c_str());
            qDebug("strategy = %d", psg.demand.strategy);
            qDebug("durlim = %d", psg.demand.durLimit);
            trafficMap.getStrategy(psg);
            trafficMap.restoreEdge();
            if (psg.state.totDur == -1)
            {
                QMessageBox::information(this, "该时间限制内无合法路径", "请放宽时间限制。");
                deletePassenger(i);
                break;
            }
            psg.state.label->~QLabel();
            psg.state.label = new QLabel("fuck");
            psg.state.label->setPixmap(QPixmap(":/images/stay.png"));
            psg.state.label->setParent(this);
            psg.state.label->show();
            int x = trafficMap.cityCoor[trafficMap.cityNameToNo[psg.demand.startCity]].x;
            int y = trafficMap.cityCoor[trafficMap.cityNameToNo[psg.demand.startCity]].y;
            psg.state.label->setGeometry(x + xOffset, y + yOffset, 42, 42);
            psg.state.remainSlot = dateTime->secsTo(psg.demand.startDateTime) / 3600 - 1;
            psg.state.nextEdgeIndex = psg.state.revPath.size() - 1;
            psg.state.anime = NULL;
            qDebug("starttime = %s", psg.demand.startDateTime.toString().toStdString().c_str());
            qDebug("remainSlot = %d", psg.state.remainSlot);
            emit updatePassengerListWidget();
            psg.changedDemand.changeFlag = false;
            continue;
        }
        qDebug("i = %d %d %d", psg.state.remainSlot, psg.state.nextEdgeIndex);
        if (psg.state.nextEdgeIndex >= -1)
        {
            if (psg.state.remainSlot <= 0)
            {
                if (psg.state.nextEdgeIndex == -1)
                {
                    fLog << dateTime->toString("yyyy年MM月dd日hh时：").toStdString() << "旅客" << psg.demand.name
                         << "成功抵达" << psg.demand.destCity << endl;
                    int remainingTime = pause();
                    QMessageBox box;
                    box.setWindowTitle(tr("旅行模拟完毕"));
                    box.setIcon(QMessageBox::Warning);
                    box.setText("旅客" + QString::fromStdString(psg.demand.name) + "模拟完毕，即将删除该旅客。");
                    box.setStandardButtons(QMessageBox::Ok);
                    box.exec();
                    deletePassenger(i);
                    i--;
                    start(remainingTime);
                }
                else
                {
                    switch (trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].kind)
                    {
                    case TrafficMap::BUS :
                        psg.state.label->setText(QString::fromStdString("BUS"));
                        break;
                    case TrafficMap::TRAIN :
                        psg.state.label->setText(QString::fromStdString("TRAIN"));
                        break;
                    case TrafficMap::PLANE :
                        psg.state.label->setText(QString::fromStdString("PLANE"));
                        break;
                    case TrafficMap::DEFAULT :
                        psg.state.label->setText(QString::fromStdString("STAY"));
                        break;
                    }
                    switch(trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].kind)
                    {
                    case TrafficMap::BUS:
                        psg.state.label->setPixmap(QPixmap(":/images/bus.png"));
                        break;
                    case TrafficMap::DEFAULT:
                        psg.state.label->setPixmap(QPixmap(":/images/stay.png"));
                        break;
                    case TrafficMap::PLANE:
                        psg.state.label->setPixmap(QPixmap(":/images/plane.png"));
                        break;
                    case TrafficMap::TRAIN:
                        psg.state.label->setPixmap(QPixmap(":/images/train.png"));
                        break;
                    }

                    psg.state.anime = new QPropertyAnimation(psg.state.label, "geometry");
                    int startCity = trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].depart.city;
                    int destCity = trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].arrive.city;
                    qDebug("startcity = %d destcity = %d ,dur = %d", startCity, destCity, trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].dur);
                    psg.state.anime->setDuration(trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].dur * 1000);
                    psg.state.anime->setStartValue(QRect(trafficMap.cityCoor[startCity].x + xOffset, trafficMap.cityCoor[startCity].y + yOffset, 42, 42));
                    psg.state.anime->setEndValue(QRect(trafficMap.cityCoor[destCity].x + xOffset, trafficMap.cityCoor[destCity].y + yOffset, 42, 42));
                    psg.state.anime->start();
                    psg.state.remainSlot = trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].dur;
                    if (trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].kind != TrafficMap::DEFAULT)
                    fLog << dateTime->toString("yyyy年MM月dd日hh时：").toStdString() << "旅客" << psg.demand.name
                         << "搭乘" << trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].vehicleNo
                         << "从" << trafficMap.cityNoToName[startCity] << "出发，目的地" << trafficMap.cityNoToName[destCity]
                         << "，预计耗时" <<trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].dur << "小时" << endl;
                    psg.state.nextEdgeIndex --;
                }
            }
            psg.state.remainSlot --;
        }
    }
    emit selectedPassengerChanged();
    qDebug("leave processingTimeout");
}

void MainWindow::onSelectedPassengerChanged(void)
{
    qDebug("enter onSelectedPassengerChanged");
    if (selectedPassenger == -1)
    {
        changeAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }
    else
    {
        changeAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }

    qDebug("leave onSelectedPassengerChanged");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/images/logo.png"));
    addDialog = NULL;
    setFixedSize(MainWindowWidth, MainWindowHeight);
    createActions();
    createMenus();
    mainWidget = new MainWidget;
    setCentralWidget(mainWidget);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QPixmap(":/images/background.png"));
    setPalette(palette);

    timer = new QTimer;
    timer->start(TIMER);
    dateTime = new QDateTime(QDateTime::currentDateTime());

    connect(timer, SIGNAL(timeout()), this, SLOT(processingTimeout()));
    connect(timer, SIGNAL(timeout()),mainWidget, SIGNAL(updateInfoTextEdit()));
    connect(this, SIGNAL(updatePassengerListWidget()), mainWidget, SIGNAL(updatePassengerListWidget()));
    connect(this, SIGNAL(selectedPassengerChanged()), mainWidget, SIGNAL(selectedPassengerChanged()));
    connect(this, SIGNAL(selectedPassengerChanged()), this, SLOT(onSelectedPassengerChanged()));
    connect(this, SIGNAL(updateTime()), mainWidget, SIGNAL(updateTime()));

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(2);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
//    wait.wait();
    animation->pause();
//    wait.wait(500000);
    animation->resume();
}


MainWindow::~MainWindow()
{

}
