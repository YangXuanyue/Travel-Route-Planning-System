#include "mapWidget.h"
#include "mainWindow.h"

MapWidget::MapWidget(QWidget *parent)
{
    setFixedSize(711, 590);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00, 0x00, 0x00, 0x00));
    setPalette(pal);
}

void MapWidget::draw(QPainter *painter)
{
    QPixmap mapPixmap(":/images/map.png");
    painter->drawPixmap(0, 0, mapPixmap);

}

void MapWidget::drawEdge(QPainter *painter)
{
//    qDebug("enter drawEdge");

    if (passengers.empty() || selectedPassenger == -1)
        return;

    Passenger passenger(passengers.at(selectedPassenger) );
    for (auto it = passenger.state.revPath.rbegin(); it != passenger.state.revPath.rend(); ++it)
    {
        if (trafficMap.edges[*it].depart.city == trafficMap.edges[*it].arrive.city)
            continue;
        int x1 = trafficMap.cityCoor[trafficMap.edges[*it].depart.city].x;
        int y1 = trafficMap.cityCoor[trafficMap.edges[*it].depart.city].y;
        int x2 = trafficMap.cityCoor[trafficMap.edges[*it].arrive.city].x;
        int y2 = trafficMap.cityCoor[trafficMap.edges[*it].arrive.city].y;    
        float l = 7;

        float a = 0.5;
        float x3 = x2 - l * cos(atan2((y2 - y1) , (x2 - x1)) - a);
        float y3 = y2 - l * sin(atan2((y2 - y1) , (x2 - x1)) - a);
        float x4 = x2 - l * sin(atan2((x2 - x1) , (y2 - y1)) - a);
        float y4 = y2 - l * cos(atan2((x2 - x1) , (y2 - y1)) - a);
        painter->drawLine(x2,y2,x3,y3);
        painter->drawLine(x2,y2,x4,y4);
        painter->drawLine(x1, y1, x2, y2);
    }

//    qDebug("leave drawEdge");
    return;
}

void MapWidget::drawCity(QPainter *painter)
{
//    qDebug("enter drawCity");

    if (passengers.empty() || selectedPassenger == -1)
        return;
    Passenger passenger(passengers.at(selectedPassenger));
    if (passenger.state.revPath.empty())
        return;
    QPixmap cityPixmap(":/images/spot.png");
    QPixmap cityPixmapSmall(":/images/spot_s.png");
    for (auto it = passenger.state.revPath.rbegin(); it != passenger.state.revPath.rend(); ++it)
    {
        int i = trafficMap.edges[*it].depart.city;
        painter->drawPixmap(trafficMap.cityCoor[i].x - 5, trafficMap.cityCoor[i].y - 16, cityPixmapSmall);
    }
    int i = trafficMap.edges[passenger.state.revPath[0]].arrive.city;
    painter->drawPixmap(trafficMap.cityCoor[i].x - 5, trafficMap.cityCoor[i].y - 16, cityPixmapSmall);

    int edgeIndex = (passenger.state.nextEdgeIndex == passenger.state.revPath.size() - 1) ? passenger.state.nextEdgeIndex : (passenger.state.nextEdgeIndex + 1);
    if (trafficMap.edges[passenger.state.revPath[edgeIndex]].arrive.city == trafficMap.edges[passenger.state.revPath[edgeIndex]].depart.city)
    {
        i = trafficMap.edges[passenger.state.revPath[edgeIndex]].arrive.city;
        painter->drawPixmap(trafficMap.cityCoor[i].x - 5, trafficMap.cityCoor[i].y - 16, cityPixmapSmall);
    }
    else
    {
        i = trafficMap.edges[passenger.state.revPath[edgeIndex]].arrive.city;
        painter->drawPixmap(trafficMap.cityCoor[i].x - 13, trafficMap.cityCoor[i].y - 41, cityPixmap);
        i = trafficMap.edges[passenger.state.revPath[edgeIndex]].depart.city;
        painter->drawPixmap(trafficMap.cityCoor[i].x - 13, trafficMap.cityCoor[i].y - 41, cityPixmap);
    }

//    qDebug("leave drawCity");
    return;
}



void MapWidget::paintEvent(QPaintEvent *event)
{
/*    QImage image(size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);

    painter.initFrom(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.eraseRect(rect());
    draw(&painter);
    painter.end();

    QPainter widgetPainter(this);
    widgetPainter.drawImage(0, 0, image);

 //   drawEdge(f)*/
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    draw(&painter);

    QPainter edgePainter(this);
    edgePainter.setRenderHint(QPainter::Antialiasing, true);

    edgePainter.setPen(QPen(QColor(255, 0, 72), 4, Qt::SolidLine, Qt::RoundCap));
    drawEdge(&edgePainter);

    QPainter cityPainter(this);
    cityPainter.setRenderHint(QPainter::Antialiasing, true);
    drawCity(&cityPainter);
}
