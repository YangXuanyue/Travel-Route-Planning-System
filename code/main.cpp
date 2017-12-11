#include <QApplication>
#include <QFont>
#include <vector>
#include <QTimer>
#include <QDateTime>
#include <QTextCodec>
#include "mainWindow.h"

MainWindow* w;

ofstream fLog;

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    fLog.open("travelAgent.log");
    if (!fLog.is_open())
    {
        qDebug("open travelAgent.log failed.");
    }
    QApplication a(argc, argv);
    QFont font("Microsoft YaHei", 10);
    a.setFont(font);
    w = new MainWindow();
    w->show();
    return a.exec();
}
