#include "changeDialog.h"
#include "mainWindow.h"
void ChangeDialog::getData(Passenger& psg)
{
    qDebug("enter getData");

    psg.changedDemand.startCity = startCityComboBox->currentText().toStdString();
    psg.changedDemand.destCity = destCityComboBox->currentText().toStdString();
    psg.changedDemand.strategy = (Strategy) strategyButtonGroup->checkedId();
    psg.changedDemand.startDateTime.setDate(w->dateTime->addSecs(3600 * (psg.state.remainSlot + 2)).date());
    psg.changedDemand.startDateTime.setTime(w->dateTime->addSecs(3600 * (psg.state.remainSlot + 2)).time());
    psg.changedDemand.startTime = psg.changedDemand.startDateTime.toString("hh").toInt();
    if (psg.changedDemand.strategy == LIM_DUR_MIN_COST)
        psg.changedDemand.durLimit = durLimLineEdit->text().toInt();

    for (int i = 0; i < CITY_NUM; i ++)
    {
        if (passCityCheckBox[i]->isChecked())
        {
            psg.changedDemand.passCities.push_back(trafficMap.cityNoToName[i]);
        }
    }
    qDebug("leave getData");
    return;
}

void ChangeDialog::onStrategyChanged(bool)
{
    qDebug("onStrategyChanged %d\n", strategyButtonGroup->checkedId());
    switch(strategyButtonGroup->checkedId())
    {
    case 0:
    case 1:
        durLimLineEdit->setEnabled(false);
        break;
    case 2:
        durLimLineEdit->setEnabled(true);
        break;
    }
    return;
}

ChangeDialog::ChangeDialog(QWidget *parent)
{
    qDebug("enter ChangeDialog");
    if (parent)
        setWindowIcon(parent->windowIcon());
    //设置图标
    Passenger& psg(passengers[selectedPassenger]);
    setWindowTitle("更改旅客");
    nameLabel = new QLabel("请输入旅客姓名：");
    startCityLabel = new QLabel("下一站出发城市：");
    destCityLabel = new QLabel("请输入目的城市：");
    nameLineEdit = new QLineEdit(QString::fromStdString(psg.demand.name));
    nameLineEdit->setEnabled(false);
    startCityComboBox = new QComboBox;
    destCityComboBox = new QComboBox;
    startCityComboBox->setFixedSize(nameLineEdit->sizeHint());
    destCityComboBox->setFixedSize(nameLineEdit->sizeHint());
    for (int i =  0; i < CITY_NUM; i ++)
    {
        startCityComboBox->addItem(QString::fromStdString(trafficMap.cityNoToName[i]));
        destCityComboBox->addItem(QString::fromStdString(trafficMap.cityNoToName[i]));
    }

    if (psg.state.nextEdgeIndex == -1)
        startCityComboBox->setCurrentIndex(trafficMap.edges[psg.state.revPath[0]].arrive.city);
    else
        startCityComboBox->setCurrentIndex(trafficMap.edges[psg.state.revPath[psg.state.nextEdgeIndex]].depart.city);
    startCityComboBox->setEnabled(false);
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameLineEdit);
/*    QHBoxLayout *startTimeLayout = new QHBoxLayout;
    startTimeLayout->addWidget(startTimeLabel);
    startTimeLayout->addWidget(startTimeDateTimeEdit);*/

    QHBoxLayout *startCityLayout = new QHBoxLayout;
    startCityLayout->addWidget(startCityLabel);
    startCityLayout->addWidget(startCityComboBox);


    QHBoxLayout *destCityLayout = new QHBoxLayout;
    destCityLayout->addWidget(destCityLabel);
    destCityLayout->addWidget(destCityComboBox);

    QVBoxLayout *passengerLayout = new QVBoxLayout;
    passengerLayout->addLayout(nameLayout);
//    passengerLayout->addLayout(startTimeLayout);
    passengerLayout->addLayout(startCityLayout);
    passengerLayout->addLayout(destCityLayout);

    passengerGroupBox = new QGroupBox("旅客信息");
    passengerGroupBox->setLayout(passengerLayout);
    //旅客信息GroupBox
    strategyGroupBox = new QGroupBox("策略选择");
    minCostRadioButton = new QRadioButton("最少花费策略");
    minDurRadioButton = new QRadioButton("最短时间策略");
    limDurMinCostRadioButton = new QRadioButton("混合策略");
    durLimLineEdit = new QLineEdit;
    durLimLineEdit->setPlaceholderText("请输入最大时间限制");
    durLimLineEdit->setValidator(new QIntValidator(0, 1000, this));
    strategyButtonGroup = new QButtonGroup(this);
    strategyButtonGroup->addButton(minCostRadioButton, 0);
    strategyButtonGroup->addButton(minDurRadioButton, 1);
    strategyButtonGroup->addButton(limDurMinCostRadioButton, 2);
    switch(psg.demand.strategy)
    {
    case MIN_COST:
        minCostRadioButton->setChecked(true);
        break;
    case MIN_DUR:
        minDurRadioButton->setChecked(true);
        break;
    case LIM_DUR_MIN_COST:
        limDurMinCostRadioButton->setChecked(true);
        durLimLineEdit->setEnabled(true);
        QString tmp;
        tmp.setNum(psg.demand.durLimit);
        durLimLineEdit->setText(tmp);
        break;
    }
    connect(minCostRadioButton, SIGNAL(clicked(bool)), this, SLOT(onStrategyChanged(bool)));
    connect(minDurRadioButton, SIGNAL(clicked(bool)), this, SLOT(onStrategyChanged(bool)));
    connect(limDurMinCostRadioButton, SIGNAL(clicked(bool)), this, SLOT(onStrategyChanged(bool)));

    QVBoxLayout *strategyLayout = new QVBoxLayout;
    strategyLayout->addWidget(minCostRadioButton);
    strategyLayout->addWidget(minDurRadioButton);
    QHBoxLayout *limDurMinCostLayout = new QHBoxLayout;
    limDurMinCostLayout->addWidget(limDurMinCostRadioButton);
    limDurMinCostLayout->addSpacing(20);
    limDurMinCostLayout->addWidget(durLimLineEdit);
    strategyLayout->addLayout(limDurMinCostLayout);
    strategyLayout->addStretch(1);
    strategyGroupBox->setLayout(strategyLayout);
    //策略选择GroupBox

    QGridLayout *passCityLayout = new QGridLayout;
    for (int i = 0; i < CITY_NUM; i ++)
    {
        passCityCheckBox[i] = new QCheckBox(QString::fromStdString(trafficMap.cityNoToName[i]), this);
        passCityLayout->addWidget(passCityCheckBox[i], i / 2, i % 2);
    }
    bool flag = false;
//    for_each(psg.demand.passCities.begin(), psg.demand.passCities.end(), [flag](string& s){passCityCheckBox[trafficMap.cityNameToNo[s]]->setChecked(true); flag = true;});
    for (int i = 0; i < psg.demand.passCities.size(); i ++)
    {
        passCityCheckBox[trafficMap.cityNameToNo[psg.demand.passCities[i]]]->setChecked(true);
        flag = true;
    }
    passCityGroupBox = new QGroupBox("途经城市");
    passCityGroupBox->setLayout(passCityLayout);

    if (!flag)
        passCityGroupBox->hide();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(passengerGroupBox);
    leftLayout->addWidget(strategyGroupBox);
    leftLayout->addWidget(passCityGroupBox);

    okButton = new QPushButton("确定");
    cancelButton = new QPushButton("取消");
    advanceButton = new QPushButton("高级");
    advanceButton->setCheckable(true);
    advanceButton->setChecked(flag);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addSpacing(10);
    rightLayout->addWidget(okButton);
    rightLayout->addWidget(cancelButton);
    rightLayout->addSpacing(20);
    rightLayout->addWidget(advanceButton);
    rightLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(advanceButton, SIGNAL(toggled(bool)), passCityGroupBox, SLOT(setVisible(bool)));
    qDebug("leave ChangeDialog");
    return;
}
