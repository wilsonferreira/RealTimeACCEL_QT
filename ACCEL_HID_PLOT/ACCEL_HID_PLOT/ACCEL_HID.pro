#-------------------------------------------------
#
# Project created by QtCreator 2015-02-21T13:16:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ACCEL_HID
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qcustomplot.cpp

HEADERS  += mainwindow.h \
        qcustomplot.h

FORMS    += mainwindow.ui


unix:!macx: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lhidapi-hidraw

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/libhidapi-hidraw.a


unix:!macx: LIBS += -lusb
