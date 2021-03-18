#-------------------------------------------------
#
# Project created by QtCreator 2021-03-14T20:43:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    VideoPlayer.cpp \
    DecLabel.cpp

HEADERS  += mainwindow.h \
    VideoPlayer.h \
    DecLabel.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/
LIBS += -lavformat -lavcodec -lavutil -lswresample -lswscale -pthread
