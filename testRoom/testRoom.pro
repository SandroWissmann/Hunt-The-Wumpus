 
QT += widgets
QT += testlib
QT += gui
CONFIG += c++17 console

TEMPLATE = app

include (../hunt_the_wumpus-src.pri)

APP_PATH = ../src
INCLUDEPATH += $$APP_PATH
DEPENDPATH += $$APP_PATH

HEADERS += \
    testroom.h

SOURCES += \
    testroom.cpp
