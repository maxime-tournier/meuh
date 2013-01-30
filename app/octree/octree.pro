TEMPLATE = app
TARGET = bin/octree

CONFIG += qt
QT += core gui opengl xml 

PKGCONFIG += luabind lua5.1

LIBS += -lmath -lphys -lgeo -ltool -lgui -lgl -lscript -lqglviewer-qt4 -lcore -lcoll
LIBS += -lgomp 
SOURCES += main.cpp 
