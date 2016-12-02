#-------------------------------------------------
#
# Project created by QtCreator 2016-12-02T03:45:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PasswordManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client.cpp \
    message.cpp \
    info.cpp

HEADERS  += mainwindow.h \
    client.h \
    libraries.h \
    message.h \
    info.h

FORMS    += mainwindow.ui
LIBS += -lws2_32
QMAKE_CXXFLAGS += -lws2_32
