#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QAction>
#include<QMenu>
#include<QMenuBar>
#include <QDebug>
#include <QPalette>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDateTime>
#include <QWaitCondition>
#include <fstream>
#include <cstdlib>
#include "structs.h"
#include <QProcess>
#include <QDir>
#define MainWindowHeight 720
#define MainWindowWidth 1080

class MainWidget;
class AddDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QDateTime* dateTime;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void about(void);
    void add(void);
    void change(void);
    void deleteSlot();
    void openLog();
    void processingTimeout(void);
    void onSelectedPassengerChanged(void);

signals:
    void updatePassengerListWidget(void);
    void selectedPassengerChanged(void);
    void updateTime(void);


private:
    void createActions(void);
    void createMenus(void);
    void start(int);
    void deletePassenger(int selectedPassenger);
    int pause(void);
    MainWidget *mainWidget;
    AddDialog *addDialog;
    QAction *addAction;
    QAction *changeAction;
    QAction *deleteAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *openLogAction;
    QMenu *manageMenu;
    QMenu *helpMenu;
    QTimer * timer;


};
extern ofstream fLog;
extern MainWindow* w;
#endif // MAINWINDOW_H
