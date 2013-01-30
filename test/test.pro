

TEMPLATE = app
TARGET = bin/test

QT += opengl 
LIBS += -lcore  -lphys  -lgl -lGLEW  -lmath -lscript -lgui -lplugin -ltool

# QMAKE_CXX = /usr/lib/gcc-snapshot/bin/g++


CONFIG -= silent
SOURCES += main.cpp test.cpp #math.cpp #test_qp.cpp #concept.cpp #script.cpp #lcp.cpp 

# QMAKE_CXX = /usr/lib/gcc-snapshot/bin/g++


PKGCONFIG += luabind lua5.1

SOURCES = math.cpp

SOURCES = asf.cpp

SOURCES = main.cpp
