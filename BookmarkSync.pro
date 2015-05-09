#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T15:08:30
#
#-------------------------------------------------

QT       += core gui webkitwidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookmarkSync
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    searchlistview.cpp \
    bookmarkmodel.cpp

HEADERS  += mainwindow.h \
    searchlistview.h \
    bookmarkmodel.h

FORMS    += mainwindow.ui \
    searchlistview.ui

RESOURCES += \
    resources.qrc
