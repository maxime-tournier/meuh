

TEMPLATE = app
TARGET = lcp


include(../common.pro)

# CONFIG += debug

LIBS += -lcore -lmath -lpthread  -lgomp -lscript -lluabind
INCLUDEPATH += /usr/include/lua5.1
SOURCES += lcp.cpp #lcp.cpp

