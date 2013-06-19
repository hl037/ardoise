# -------------------------------------------------
# Project created by QtCreator 2010-08-25T13:40:51
# -------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ardoise
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ardoise.cpp \
    rectselection.cpp \
    rbouton.cpp \
    cursor.cpp \
    flowlayout.cpp \
    textinput.cpp \
    json/ljsonp.cpp \
    options.cpp \
    optionswidget.cpp \
    releasefocusfilter.cpp \
    displaywidget.cpp
HEADERS += mainwindow.h \
    ardoise.h \
    rectselection.h \
    rbouton.h \
    cursor.h \
    flowlayout.h \
    textinput.h \
    version.h \
    json/ljsonp.hpp \
    options.h \
    optionswidget.h \
    stringtotranslate.h \
    releasefocusfilter.h \
    displaywidget.h
FORMS += \
    mainwindow.ui \
    rectselection.ui \
    optionswidget.ui
OTHER_FILES += COPYING.txt \
    README.txt \
    linux/ardoise.desktop \
    windows/INSTALL.txt \
    windows/script.nsi \
    windows/conf/last.xapal \
    windows/conf/default.xapal \
    DevLine.txt \
    help_fr.html \
    help_en.html \
    help.html \
    linux/INSTALL.txt

RESOURCES += icons.qrc \
    translations.qrc
RC_FILE = "icon.rc"

CONFIG += c++11

CODECFORTR = UTF-8

TRANSLATIONS = \
   ardoise_en.ts \
   ardoise_fr.ts \
   ardoise_nl.ts

#DEFINES += NO_DEBUG

