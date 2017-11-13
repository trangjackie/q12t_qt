#-------------------------------------------------
#
# Project created by QtCreator 2017-11-11T02:08:58
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = q12t_sram_test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

# for rpi lcd with user pi
target.path=/home/pi/qtapp
INSTALLS+=target


