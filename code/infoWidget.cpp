#include <infoWidget.h>
#include <mainWindow.h>
void InfoWidget::updatePassengerListWidget(void)
{

    qDebug("enter updatePassengerListWidget");
    bool flag = false;
    passengerListWidget->clear();
    for (int i = 0; i < passengers.size(); i ++)
    {
            passengerListWidget->addItem(QString::fromStdString(passengers[i].demand.name));
            flag = true;
    }
    if (flag)
        passengerListWidget->setCurrentRow(0);
    else
        passengerListWidget->setCurrentRow(-1);
    qDebug("updatePassengerListWidget complete");
}


void InfoWidget::updateTime()
{
    qDebug("enter updateTime");
    timeLCDNumber->display(w->dateTime->toString("hh:00:00"));
    dateLCDNumber->display(w->dateTime->toString("yyyy-MM-dd"));
    qDebug("leave updateTime");
}

void InfoWidget::onCurrentRowChanged(int row)
{
    qDebug("enter onCurrentRowChanged %d", row);
    if (row == -1)
        return;
    if (selectedPassenger >= 0 && selectedPassenger < passengers.size())
        passengers[selectedPassenger].state.label->hide();
    selectedPassenger = row;
    if (selectedPassenger >= 0 && selectedPassenger < passengers.size())
        passengers[selectedPassenger].state.label->show();
    emit w->selectedPassengerChanged();
    updateInfoTextEdit();
    qDebug("leave onCurrentRowChanged");
}

void InfoWidget::updateInfoTextEdit(void)
{
    qDebug("enter updateInfoTextEdit");
    if (selectedPassenger == -1 || passengers.empty())
        return;
    char buf[100];
    string vehicleKindToCh[4] = {"搭客车", "乘火车", "坐飞机", "逗留"};
    Passenger& psg(passengers[selectedPassenger]);

    int index = psg.state.nextEdgeIndex == psg.state.revPath.size() - 1 ? psg.state.nextEdgeIndex : psg.state.nextEdgeIndex + 1;
    infoTextEdit->setText(QString::fromStdString("旅客姓名：" + passengers[selectedPassenger].demand.name));
    infoTextEdit->append("花费总时间：" + QString::fromStdString(string(itoa(psg.state.totDur, buf, 10))) + "小时");
    infoTextEdit->append("花费总金钱：" + QString::fromStdString(string(itoa(psg.state.totCost, buf, 10))) + "元");
    infoTextEdit->append("");
    if (psg.state.nextEdgeIndex == psg.state.revPath.size() - 1)
    {
        infoTextEdit->append("当前状态：还未到出行时间");
        infoTextEdit->append("出行时间：" + psg.demand.startDateTime.toString("yyyy年MM月dd日hh时"));
        return;
    }
    TrafficMap::Edge& e(trafficMap.edges[psg.state.revPath[index]]);
    switch(e.kind)
    {
    case TrafficMap::DEFAULT:
    {
        infoTextEdit->append("当前状态：等车");
        infoTextEdit->append("当前城市：" + QString::fromStdString(trafficMap.cityNoToName[e.depart.city]));
        for (int i = index - 1; i >= 0; i --)
            if (trafficMap.edges[psg.state.revPath[i]].kind != TrafficMap::DEFAULT)
            {
                infoTextEdit->append("等候车次：" + QString::fromStdString(trafficMap.edges[psg.state.revPath[i]].vehicleNo));
                infoTextEdit->append("出发城市：" + QString::fromStdString(trafficMap.cityNoToName[trafficMap.edges[psg.state.revPath[i]].depart.city]));
                if (trafficMap.edges[psg.state.revPath[i]].depart.time < w->dateTime->toString("hh").toInt())
                    infoTextEdit->append("出发时间：次日" +  QString::fromStdString(string(itoa(trafficMap.edges[psg.state.revPath[i]].depart.time, buf, 10))) + "时");
                else
                    infoTextEdit->append("出发时间：今日" +  QString::fromStdString(string(itoa(trafficMap.edges[psg.state.revPath[i]].depart.time, buf, 10))) + "时");
                infoTextEdit->append("到达城市：" + QString::fromStdString(trafficMap.cityNoToName[trafficMap.edges[psg.state.revPath[i]].arrive.city]));
                infoTextEdit->append("交通方式：" + QString::fromStdString(vehicleKindToCh[trafficMap.edges[psg.state.revPath[i]].kind]));
                break;
            }
        break;
    }
    default:
        infoTextEdit->append("交通方式：" + QString::fromStdString(vehicleKindToCh[e.kind]));
        infoTextEdit->append("搭乘车次：" + QString::fromStdString(e.vehicleNo));
        infoTextEdit->append("出发城市：" + QString::fromStdString(trafficMap.cityNoToName[e.depart.city]));
        infoTextEdit->append("目的城市：" + QString::fromStdString(trafficMap.cityNoToName[e.arrive.city]));
        infoTextEdit->append("抵达时间：" + QString::fromStdString(string(itoa(e.arrive.time, buf, 10))) + "时");
        break;
    }
    qDebug("leave updateInfoTextEdit");
    return;
}

InfoWidget::InfoWidget(QWidget* parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00, 0x00, 0x00, 0x00));
    setPalette(pal);

    timeLCDNumber = new QLCDNumber(8);
    dateLCDNumber = new QLCDNumber(10);
    timeLCDNumber->setSegmentStyle(QLCDNumber::Flat);
    timeLCDNumber->setFixedHeight(50);
    timeLCDNumber->setLineWidth(0);
    timeLCDNumber->setStyleSheet("color:rgb(6, 12 ,41);");
    dateLCDNumber->setSegmentStyle(QLCDNumber::Flat);
    dateLCDNumber->setFixedHeight(50);
    dateLCDNumber->setLineWidth(0);
    dateLCDNumber->setStyleSheet("color:rgb(6, 12 ,41);");
    passengerLabel = new QLabel("旅客");
    passengerListWidget = new QListWidget;
    passengerListWidget->setFixedWidth(200);
    infoLabel = new QLabel("旅客信息");
    infoTextEdit = new QTextEdit;
    infoTextEdit->setFixedWidth(200);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(dateLCDNumber);
    mainLayout->addWidget(timeLCDNumber);
    mainLayout->addWidget(passengerLabel);
    mainLayout->addWidget(passengerListWidget);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(infoTextEdit);
    mainLayout->setAlignment(Qt::AlignHCenter);
    setLayout(mainLayout);
   connect(passengerListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentRowChanged(int)));
}

