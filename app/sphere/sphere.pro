TEMPLATE = app
TARGET = sphere

include(../../common.pro)

CONFIG += qt
QT += core xml gui opengl

LIBS += -lqglviewer-qt4  -lcore -lmath  -lmocap -lgl -lphys -lGLEW  -lgeo -lcontrol -ltool -lgui -lgomp 

INCLUDEPATH += /usr/include/lua5.1
LIBS += -lscript -llua5.1 -lluabind

LIBS += -lboost_program_options

HEADERS +=  sphere.h
SOURCES +=  sphere.cpp
