#ifndef INFOWIDGET_H
#define INFOWIDGET_H
#include <QLabel>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLayout>
#include <QPalette>
#include <QLCDNumber>
#include "structs.h"




class InfoWidget : public QWidget
{
    Q_OBJECT
public:
    InfoWidget(QWidget *parent = 0);
private slots:
    void updatePassengerListWidget(void);
    void updateTime(void);
    void updateInfoTextEdit(void);
    void onCurrentRowChanged(int);
private:
    QLCDNumber *timeLCDNumber;
    QLCDNumber *dateLCDNumber;
    QLabel *passengerLabel;
    QListWidget *passengerListWidget;
    QLabel *infoLabel;
    QTextEdit *infoTextEdit;
};

#endif // INFOWIDGET_H
