#-------------------------------------------------
#
# Project created by QtCreator 2016-06-01T15:43:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TravelAgent
TEMPLATE = app


SOURCES += main.cpp\
    debug.cpp \
    strategies.cpp \
    structs.cpp \
    addDialog.cpp \
    changeDialog.cpp \
    infoWidget.cpp \
    mainWidget.cpp \
    mainWindow.cpp \
    mapWidget.cpp

HEADERS  += \
    structs.h \
    addDialog.h \
    changeDialog.h \
    infoWidget.h \
    mainWidget.h \
    mainWindow.h \
    mapWidget.h

RESOURCES += \
    travelagent.qrc

CONFIG += c++11

DISTFILES +=
