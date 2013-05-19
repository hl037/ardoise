# -------------------------------------------------
# Project created by QtCreator 2010-08-25T13:40:51
# -------------------------------------------------

QT       += core gui xml

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
    README.txt \
    linux/ardoise.desktop \
    windows/INSTALL.txt \
    windows/script.nsi \
    windows/conf/last.xapal \
    windows/conf/default.xapal

RESOURCES += icons.qrc
RC_FILE = "icon.rc"

CONFIG += c++11

CODECFORTR = UTF-8

TRANSLATIONS = \
   ardoise_en.ts \
   ardoise_fr.ts \
   ardoise_nl.ts

#DEFINES += NO_DEBUG

