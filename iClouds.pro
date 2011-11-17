#-------------------------------------------------
#
# Project created by QtCreator 2011-08-03T20:32:52
#
#-------------------------------------------------

QT       += core gui

TARGET = iClouds
TEMPLATE = app

INCLUDEPATH += C:\OpenCV2.1\include\opencv
LIBS += C:\OpenCV2.1\lib\cv210d.lib \
    C:\OpenCV2.1\lib\cvaux210d.lib \
    C:\OpenCV2.1\lib\cxcore210d.lib \
    C:\OpenCV2.1\lib\cxts210.lib \
    C:\OpenCV2.1\lib\highgui210d.lib \
    C:\OpenCV2.1\lib\ml210d.lib \
    C:\OpenCV2.1\lib\opencv_ffmpeg210d.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    munkres.cpp \
    matrix.cpp \
    CThinPlateSpline.cpp

HEADERS  += mainwindow.h \
    munkres.h \
    matrix.h \
    CThinPlateSpline.h

FORMS    += mainwindow.ui
