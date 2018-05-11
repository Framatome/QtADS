#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T08:31:48
#
#-------------------------------------------------

# Don't forget to set QMAKEFEATURES
# Example: qmake -set QMAKEFEATURES /usr/local/qtads-1.0.0/features

QT       += core gui
CONFIG   += QtADS

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtWidgetsADSExample
TEMPLATE = app


SOURCES += main.cpp\
        qadsdialog.cpp

HEADERS  += qadsdialog.h

FORMS    += qadsdialog.ui
