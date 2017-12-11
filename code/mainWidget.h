#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QImage>
#include <QSplitter>
#include <QListView>
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include <QGraphicsOpacityEffect>


class InfoWidget;
class MapWidget;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget* parent = 0);
signals:
    void updatePassengerListWidget(void);
    void selectedPassengerChanged(void);
    void updateTime();
    void updateInfoTextEdit(void);
private:
    MapWidget *mapWidget;
    InfoWidget *infoWidget;
//    QWidget* parent;
};

#endif // MAINWIDGET_H
