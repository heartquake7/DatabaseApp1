#-------------------------------------------------
#
# Project created by QtCreator 2018-03-06T20:11:44
#
#-------------------------------------------------

QT       += core gui sql
QT       += webkit webkitwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DatabaseApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    database.cpp \
    startwindow.cpp \
    regstartwnd.cpp \
    addregdialog.cpp \
    choosevehdlg.cpp \
    chooseowndlg.cpp \
    choosetagdlg.cpp \
    accstartwnd.cpp \
    webmapdialog.cpp \
    marker.cpp \
    addaccdialog.cpp \
    infodlg.cpp \
    choosevehaccdlg.cpp

HEADERS += \
    database.h \
    startwindow.h \
    regstartwnd.h \
    addregdialog.h \
    choosevehdlg.h \
    chooseowndlg.h \
    choosetagdlg.h \
    accstartwnd.h \
    webmapdialog.h \
    marker.h \
    addaccdialog.h \
    infodlg.h \
    choosevehaccdlg.h

FORMS += \
    startwindow.ui \
    regstartwnd.ui \
    regstartwnd.ui \
    addregdialog.ui \
    choosevehdlg.ui \
    chooseowndlg.ui \
    choosetagdlg.ui \
    accstartwnd.ui \
    webmapdialog.ui \
    addaccdialog.ui \
    infodlg.ui \
    choosevehaccdlg.ui

DISTFILES +=

