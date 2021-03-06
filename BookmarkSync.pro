#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T15:08:30
#
#-------------------------------------------------

QT       += core gui webkitwidgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookmarkSync
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    searchlistview.cpp \
    bookmarkmodel.cpp \
    addnewlink.cpp \
    sqlitedatabaseadapter.cpp \
    testdatabase.cpp \
    bookmarkcombobox.cpp \
    bookmarkvaluelineedit.cpp \
    complexlineedit.cpp

HEADERS  += mainwindow.h \
    searchlistview.h \
    bookmarkmodel.h \
    addnewlink.h \
    sqlitedatabaseadapter.h \
    globaltypes.h \
    testdatabase.h \
    bookmarkcombobox.h \
    bookmarkvaluelineedit.h \
    complexlineedit.h

FORMS    += mainwindow.ui \
    searchlistview.ui \
    addnewlink.ui \
    complexlineedit.ui

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS += -std=c++11 -m32
