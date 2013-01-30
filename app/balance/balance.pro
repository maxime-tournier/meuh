TEMPLATE = app
TARGET = bin/balance

CONFIG += qt
QT += core gui opengl xml 

PKGCONFIG += luabind lua5.1

LIBS += -lgui -ltool -lqglviewer-qt4 -lphys -lscript -ltool -lmocap -lcore -lgl -lmath -lgeo -lcoll

SOURCES += \
    main.cpp \
    viewer.cpp \
    setup.cpp \
    control.cpp \
    collisions.cpp \
