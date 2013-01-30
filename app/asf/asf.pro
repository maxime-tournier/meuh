
TEMPLATE = app
TARGET = bin/asf

CONFIG += qt
QT += xml gui opengl core

LIBS += -lqglviewer-qt4 -lgl -lGLEW -lmath -lcore -ltool -lgui -lpthread -lgeo -lmocap -lphys -lscript -lgomp

PKGCONFIG += lua5.1

SOURCES += \
    main.cpp \
    viewer.cpp \

