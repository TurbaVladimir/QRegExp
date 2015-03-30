#-------------------------------------------------
#
# Project created by QtCreator 2014-11-04T18:47:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qregexp_tester
TEMPLATE = app

DEFINES += "STORAGE_FILENAME=storage"


SOURCES += main.cpp\
	mainwindow.cpp \
    storagewindow.cpp \
    modificatorswindow.cpp

HEADERS  += mainwindow.h \
    storagewindow.h \
    modificatorswindow.h

FORMS    += mainwindow.ui \
    storage.ui \
    modificators.ui

RESOURCES +=
