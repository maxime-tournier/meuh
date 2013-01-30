

TEMPLATE = app
TARGET = qp


include(../common.pro)

#CONFIG += debug


LIBS += -lcore -lmath -lpthread 

SOURCES += qp.cpp #lcp.cpp

