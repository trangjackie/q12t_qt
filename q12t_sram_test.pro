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
        mainwindow.cpp \
    keyboard/keyboard.cpp \
    keyboardqwerty/keyboardqwerty.cpp \
    uart/settingsdialog.cpp

HEADERS  += mainwindow.h \
    keyboard/keyboard.h \
    keyboard/ui_keyboard.h \
    keyboardqwerty/keyboardqwerty.h \
    uart/settingsdialog.h

FORMS    += mainwindow.ui \
    keyboard/keyboard.ui \
    keyboardqwerty/keyboardqwerty.ui \
    uart/settingsdialog.ui \
    bitmap_all.ui

# for rpi lcd with user pi
target.path=/home/pi/qtapp
INSTALLS+=target

DISTFILES +=

RESOURCES += \
    icons/icons.qrc


