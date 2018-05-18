#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T10:44:20
#
#-------------------------------------------------

QT       += core gui   network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bir_Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    setting.cpp

HEADERS  += mainwindow.h \
    setting.h

FORMS    += mainwindow.ui \
    setting.ui

LIBS += -lSDL
LIBS += `pkg-config opencv --libs`
