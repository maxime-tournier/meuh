
TEMPLATE = app
TARGET = bin/sofa

CONFIG += qt
QT += xml gui opengl core

PKGCONFIG += lua5.1

LIBS +=  -ltool -lgui -lscript -lluabind -lcore -lboost_program_options

SOURCES += main.cpp
