#-------------------------------------------------
#
# Project created by QtCreator 2021-03-14T20:43:16
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    VideoPlayer.cpp \
    DecLabel.cpp \
    AudioPlayer.cpp \
    AudioOutput.cpp \
    VideoScale.cpp \
    VideoPage.cpp \
    ScalePage.cpp

HEADERS  += mainwindow.h \
    VideoPlayer.h \
    DecLabel.h \
    AudioPlayer.h \
    AudioOutput.h \
    VideoScale.h \
    VideoPage.h \
    ScalePage.h

FORMS    += \
    mainwindow.ui

INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib/ -lavformat -lavcodec -lavutil -lswresample -lswscale -pthread
