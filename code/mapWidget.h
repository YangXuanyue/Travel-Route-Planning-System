#ifndef MAPWIDGET_H
#define MAPWIDGET_H
#include <QPainter>
#include <QFrame>
#include <QImage>
#include <QWidget>
#include <QLayout>
#include <QIcon>
#include <QPixmap>
#include <QRectF>
#include <QGraphicsBlurEffect>
class MapWidget : public QWidget
{
    Q_OBJECT
public:
    MapWidget(QWidget* parent = 0);
    void draw(QPainter *painter);
    void drawEdge(QPainter *painter);
    void drawCity(QPainter *painter);
private:
    void paintEvent(QPaintEvent *event = 0);
};
#endif // MAPWIDGET_H
