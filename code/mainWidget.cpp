#include "mainWidget.h"
#include "mapWidget.h"
#include "infoWidget.h"

MainWidget::MainWidget(QWidget *parent)
{
    if(parent)
        setWindowIcon(parent->windowIcon());
    mapWidget = new MapWidget;
    infoWidget= new InfoWidget;

    QGridLayout *infoLayout = new QGridLayout;
    infoLayout->addWidget(infoWidget);
    infoLayout->setMargin(15);
    QGroupBox *infoGroupBox = new QGroupBox("信息");
    infoGroupBox->setLayout(infoLayout);


    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mapWidget);
    mainLayout->addSpacing(50);
    mainLayout->addWidget(infoGroupBox);

    setLayout(mainLayout);
    connect(this, SIGNAL(updatePassengerListWidget()), infoWidget, SLOT(updatePassengerListWidget()));
    connect(this, SIGNAL(updateInfoTextEdit()), infoWidget, SLOT(updateInfoTextEdit()));
    connect(this, SIGNAL(selectedPassengerChanged()), mapWidget, SLOT(repaint()));
    connect(this, SIGNAL(updateTime()), infoWidget, SLOT(updateTime()));
}
