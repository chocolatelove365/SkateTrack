#-------------------------------------------------
#
# Project created by QtCreator 2017-12-01T17:50:37
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SkateTrack
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        trackingwindow.cpp \
    imageformat.cpp \
    particle_filter.cpp \
    customlabel.cpp \
    trajectorywindow.cpp \
    converter.cpp \
    trackingview.cpp \
    trajectory.cpp

HEADERS += \
        trackingwindow.h \
    imageformat.h \
    particle_filter.hpp \
    customlabel.h \
    trajectorywindow.h \
    converter.h \
    trackingview.h \
    trajectory.h

FORMS += \
        trackingwindow.ui

DEPENDPATH += /opt/local/include /usr/local/include
INCLUDEPATH += /opt/local/include /usr/local/include
LIBS += -L/usr/local/Cellar/opencv/3.3.1_1/lib \
     -lopencv_core \
     -lopencv_imgproc \
    -lopencv_imgcodecs \
     -lopencv_highgui \
    -lopencv_videoio

DISTFILES += \
    config.json
