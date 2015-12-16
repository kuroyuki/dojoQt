#-------------------------------------------------
#
# Project created by QtCreator 2015-12-17T00:37:44
#
#-------------------------------------------------

QT       += core gui network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dojoApp
TEMPLATE = app


SOURCES += main.cpp\
    ../../dojoioserver.cpp \
    ../../dojologger.cpp \
    ../../dojonetwork.cpp \
    ../../dojoneuron.cpp \
    ../../dojostorage.cpp \
    ../../dojosynapse.cpp \
    ../../dojowsserver.cpp \
    widget.cpp \
    ../../dojoclient.cpp

HEADERS  += \
    ../../dojoioserver.h \
    ../../dojologger.h \
    ../../dojonetwork.h \
    ../../dojoneuron.h \
    ../../dojostorage.h \
    ../../dojosynapse.h \
    ../../dojowsserver.h \
    widget.h \
    ../../dojoclient.h

FORMS    +=

DISTFILES += \
    ../../html/app.js \
    ../../html/controllers.js \
    ../../html/services.js \
    ../../html/index.html
