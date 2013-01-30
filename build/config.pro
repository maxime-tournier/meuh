

CONFIG = release
QT = 

QMAKE_CXXFLAGS += -std=c++0x 

MEUH_PATH = $$IN_PWD/..

DEPENDPATH += $$MEUH_PATH
INCLUDEPATH += $$MEUH_PATH
LIBS += -L$$MEUH_PATH/lib

# use pkgconfig
CONFIG += link_pkgconfig

PKGCONFIG += eigen3

CONFIG += silent
CONFIG += object_with_source

DESTDIR = $$MEUH_PATH

# customization file (debug, compiler, ... )
exists(custom.pro) {
    include( custom.pro )
}

!exists(custom.pro) {
		message("no meuh/build/custom.pro, using gcc-4.6 defaults")
		QMAKE_CXX = g++-4.6
		QMAKE_CXXFLAGS += -march=native 
		QMAKE_LFLAGS += -march=native
		CONFIG += openmp
}


debug {
    CONFIG -= release
    QMAKE_EXT_OBJ = .debug$$QMAKE_EXT_OBJ
}

release {
    DEFINES += NDEBUG
}

openmp {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -lgomp
}

