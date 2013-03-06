# -------------------------------------------------
# Project created by QtCreator 2010-08-25T13:40:51
# -------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ardoise
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ardoise.cpp \
    vue.cpp \
    rectselection.cpp \
    rbouton.cpp \
    cursor.cpp \
    flowlayout.cpp
HEADERS += mainwindow.h \
    ardoise.h \
    vue.h \
    rectselection.h \
    rbouton.h \
    cursor.h \
    flowlayout.h
FORMS += \
    mainwindow.ui \
    rectselection.ui
OTHER_FILES += COPYING.txt \
    README.txt
RESOURCES += icons.qrc
RC_FILE = "icon.rc"

QMAKE_CXXFLAGS += -std=c++0x

#DEFINES += NO_DEBUG

