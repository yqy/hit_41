#-------------------------------------------------
#
# Project created by QtCreator 2012-06-10T03:09:44
#
#-------------------------------------------------

QT       += core gui

TARGET = yqytest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    CPU.cpp \
    JOYPAD.cpp \
    Mapper.cpp \
    PPUINC.cpp \
    stdafx.cpp \
    NES.cpp \
    PPU.cpp \
    myui.cpp

HEADERS  += mainwindow.h \
    mytype.h \
    CPU.h \
    CPUINC.h \
    JOYPAD.h \
    Mapper.h \
    Mapper002.h \
    PPU.h \
    PPUINC.h \
    resource.h \
    stdafx.h \
    NES.h \
    myui.h

FORMS    += mainwindow.ui \
    myui.ui
