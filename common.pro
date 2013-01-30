
# all common variables go here

QMAKE_CXXFLAGS += -std=c++0x -fopenmp
QMAKE_CXXFLAGS += -Wno-deprecated

QMAKE_LINK = g++

DEPENDPATH += $$PWD

INCLUDEPATH += $$PWD /usr/include/eigen3 
LIBS += -L$${PWD}/lib

contains(TEMPLATE, app) {
    DESTDIR=$${PWD}/bin
}

contains(TEMPLATE, lib) {
    DESTDIR=$${PWD}/lib
}

BUILD = build

debug {
    OBJECTS_DIR = $$BUILD/debug
}

release {
    OBJECTS_DIR = $$BUILD/release
    DEFINES += NDEBUG                  
}

CONFIG += warn_on

# overrides
include( $${PWD}/config.pro )

