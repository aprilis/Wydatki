#-------------------------------------------------
#
# Project created by QtCreator 2015-12-27T18:45:25
#
#-------------------------------------------------

CONFIG += qt
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Wydatki
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Manager.cpp \
    TableModel.cpp \
    Util.cpp

HEADERS  += mainwindow.h \
    Manager.h \
    Item.h \
    TableModel.h \
    Util.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++17
