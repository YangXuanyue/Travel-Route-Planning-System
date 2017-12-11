#include "addDialog.h"
#include "mainWindow.h"
void AddDialog::getData(Passenger& psg)
{
    psg.demand.startCity = startCityComboBox->currentText().toStdString();
    psg.demand.destCity = destCityComboBox->currentText().toStdString();
    psg.demand.name = nameLineEdit->text().toStdString();
    psg.demand.strategy = (Strategy) strategyButtonGroup->checkedId();
    psg.demand.startDateTime.setDate(startTimeDateTimeEdit->dateTime().addSecs(3600).date());
    psg.demand.startDateTime.setTime(startTimeDateTimeEdit->dateTime().addSecs(3600).time());
    psg.demand.startTime = startTimeDateTimeEdit->dateTime().addSecs(3600).toString("hh").toInt();
    if (psg.demand.strategy == LIM_DUR_MIN_COST)
        psg.demand.durLimit = durLimLineEdit->text().toInt();

    for (int i = 0; i < CITY_NUM; i ++)
    {
        if (passCityCheckBox[i]->isChecked())
        {
            psg.demand.passCities.push_back(trafficMap.cityNoToName[i]);
        }
    }
    return;
}

void AddDialog::onStrategyChanged(bool)
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

AddDialog::AddDialog(QWidget *parent)
{
    if (parent)
        setWindowIcon(parent->windowIcon());
    //设置图标
    setWindowTitle("添加旅客");
    nameLabel = new QLabel("请输入旅客姓名：");
    startTimeLabel = new QLabel("请选择出发时间：");
    startCityLabel = new QLabel("请选择出发城市：");
    destCityLabel = new QLabel("请选择目的城市：");
    nameLineEdit = new QLineEdit;
    startTimeDateTimeEdit = new QDateTimeEdit(*(w->dateTime));
    startCityComboBox = new QComboBox;
    destCityComboBox = new QComboBox;
    startCityComboBox->setFixedSize(nameLineEdit->sizeHint());
    destCityComboBox->setFixedSize(nameLineEdit->sizeHint());
    for (int i =  0; i < CITY_NUM; i ++)
    {
        startCityComboBox->addItem(QString::fromStdString(trafficMap.cityNoToName[i]));
        destCityComboBox->addItem(QString::fromStdString(trafficMap.cityNoToName[i]));
    }
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameLineEdit);

    QHBoxLayout *startTimeLayout = new QHBoxLayout;
    startTimeLayout->addWidget(startTimeLabel);
    startTimeLayout->addWidget(startTimeDateTimeEdit);

    QHBoxLayout *startCityLayout = new QHBoxLayout;
    startCityLayout->addWidget(startCityLabel);
    startCityLayout->addWidget(startCityComboBox);


    QHBoxLayout *destCityLayout = new QHBoxLayout;
    destCityLayout->addWidget(destCityLabel);
    destCityLayout->addWidget(destCityComboBox);

    QVBoxLayout *passengerLayout = new QVBoxLayout;
    passengerLayout->addLayout(nameLayout);
    passengerLayout->addLayout(startTimeLayout);
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
    minCostRadioButton->setChecked(true);
    durLimLineEdit->setEnabled(false);
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

    /*
    cityLabel1 = new QLabel("途经城市一：");
    cityLabel2 = new QLabel("途经城市二：");
    cityLabel3 = new QLabel("途经城市三：");
    cityComboBox1 = new QComboBox;
    cityComboBox2 = new QComboBox;
    cityComboBox3 = new QComboBox;

    QGridLayout *passingCityLayout = new QGridLayout();
    passingCityLayout->addWidget(cityLabel1, 1, 1);
    passingCityLayout->addWidget(cityComboBox1, 1, 2);
    passingCityLayout->addWidget(cityLabel2, 2, 1);
    passingCityLayout->addWidget(cityComboBox2, 2, 2);
    passingCityLayout->addWidget(cityLabel3, 3, 1);
    passingCityLayout->addWidget(cityComboBox3, 3, 2);

    passingCityGroupBox = new QGroupBox("途经城市");
    passingCityGroupBox->setLayout(passingCityLayout);

    passingCityGroupBox->hide();
    */

    QGridLayout *passCityLayout = new QGridLayout;
    for (int i = 0; i < CITY_NUM; i ++)
    {
        passCityCheckBox[i] = new QCheckBox(QString::fromStdString(trafficMap.cityNoToName[i]), this);
        passCityLayout->addWidget(passCityCheckBox[i], i / 2, i % 2);
    }
    passCityGroupBox = new QGroupBox("途经城市");
    passCityGroupBox->setLayout(passCityLayout);

    passCityGroupBox->hide();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(passengerGroupBox);
    leftLayout->addWidget(strategyGroupBox);
    leftLayout->addWidget(passCityGroupBox);

    okButton = new QPushButton("确定");
    cancelButton = new QPushButton("取消");
    advanceButton = new QPushButton("高级");
    advanceButton->setCheckable(true);
    advanceButton->setChecked(false);

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
    return;
}
