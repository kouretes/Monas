# -------------------------------------------------
# Project created by QtCreator 2009-10-24T13:37:23
# -------------------------------------------------
QT += network \
    opengl \
    xml \
    xmlpatterns \
    dbus
TARGET = MotionDesigner
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    sliderdialog.cpp \
    Robot.cpp \
    sliderinput.cpp \
    jointsliders.cpp \
    aboutdialog.cpp \
    motionexecution.cpp \
    executionthread.cpp \
    client.cpp
HEADERS += mainwindow.h \
    sliderdialog.h \
    Joint.h \
    Robot.h \
    sliderinput.h \
    jointsliders.h \
    aboutdialog.h \
    motionexecution.h \
    executionthread.h \
    client.h
FORMS += mainwindow.ui \
    aboutdialog.ui \
    motionexecution.ui
RESOURCES += icons.qrc
