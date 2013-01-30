TEMPLATE = lib
TARGET = lib/plugin.mosek 

isEmpty( MOSEK_PATH ) {
    error( "you need to set MOSEK_PATH in build/custom.pro" )
}

MOSEK_LIB = $$MOSEK_PATH/bin 
DEFINES += "MOSEK_INCLUDE=$$MOSEK_PATH/h"

LIBS += -L$$MOSEK_LIB -lmosek64
SOURCES += impl.cpp
