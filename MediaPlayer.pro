#-------------------------------------------------
#
# Project created by QtCreator 2021-04-20T17:51:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SDL2Widget.cpp

HEADERS  += mainwindow.h \
    SDL2Widget.h

FORMS    += mainwindow.ui

CONFIG += link_pkgconfig
PKGCONFIG += libavformat libavcodec libavfilter libavdevice libswresample libswscale libavutil sdl2 SDL2_image
