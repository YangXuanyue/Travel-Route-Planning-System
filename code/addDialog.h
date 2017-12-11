#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include<QDialog>
#include<QDialogButtonBox>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QString>
#include <QButtonGroup>
#include <QDebug>
#include <QCheckBox>
#include <QDateTimeEdit>

#include "structs.h"

class AddDialog : public QDialog
{
    Q_OBJECT
public:
    AddDialog(QWidget * parent = 0);
    void getData(Passenger& psg);

private  slots:
    void onStrategyChanged(bool);

private:
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *advanceButton;

    QGroupBox *passengerGroupBox;
    QLabel *nameLabel;
    QLabel *startTimeLabel;
    QLabel *startCityLabel;
    QLabel *destCityLabel;

    QLineEdit *nameLineEdit;
    QDateTimeEdit *startTimeDateTimeEdit;
    QComboBox *startCityComboBox;
    QComboBox *destCityComboBox;

    QGroupBox *strategyGroupBox;
    QButtonGroup *strategyButtonGroup;
    QRadioButton *minCostRadioButton;
    QRadioButton *minDurRadioButton;
    QRadioButton *limDurMinCostRadioButton;
    QLineEdit *durLimLineEdit;

    QGroupBox *passCityGroupBox;
    QCheckBox *passCityCheckBox[CITY_NUM];

/*    QLabel *cityLabel1;
    QLabel *cityLabel2;
    QLabel *cityLabel3;
    QComboBox *cityComboBox1;
    QComboBox *cityComboBox2;
    QComboBox *cityComboBox3;*/


};


#endif // ADDDIALOG_H
