

TEMPLATE = lib
TARGET = lib/meuh

QT = 

CONFIG += qt


# compiler setup
!win32 {
    # JAVA
    INCLUDEPATH += /usr/lib/jvm/default-java/include/
}


win32 {

    # fix includes
    QMAKE_CXXFLAGS += -nostdinc
    INCLUDEPATH += /usr/i686-w64-mingw32/include /usr/include/c++/4.6 /usr/include/c++/4.6/x86_64-w64-mingw32 /usr/include

    # static link these
    QMAKE_LFLAGS += -static-libstdc++ -static-libgcc

    # are we building a dll ?
    CONFIG += dll
    
    dll {
	DEFINES += BUILD_DLL 
    }
    
    # dont use OMP
    DEFINES += EIGEN_DONT_PARALLELIZE
    
}




# swig wrapper
meuh_wrap.cxx.commands = rm -rf meuh; mkdir meuh; swig2.0 -c++ -java -outdir meuh -package meuh meuh.i 
meuh_wrap.cxx.depends = solver.h vector.h matrix.h cg.h graph.h config.h

# java 
class.depends = meuh/*.java 
class.commands = javac meuh/*.java

# jar
meuh.jar.depends = class
meuh.jar.commands = jar cvf meuh.jar meuh

QMAKE_CLEAN += meuh_wrap.cxx -r meuh meuh.jar

SOURCES += solver.cpp cg.cpp graph.cpp config.cpp vector.cpp matrix.cpp meuh_wrap.cxx

PHYS = $$MEUH_PATH/phys
CORE = $$MEUH_PATH/core

SOURCES += \
    $$PHYS/dof.cpp \ 
    $$PHYS/constraint/bilateral.cpp \
    $$PHYS/solver/task.cpp \
    $$PHYS/solver/test.cpp \
    $$PHYS/error.cpp \

SOURCES += \
    $$CORE/log.cpp \
    $$CORE/error.cpp \
    $$CORE/thread.cpp \
    
ECLIPSE = ~/workspace

eclipse.depends = meuh.jar $$LIBNAME
eclipse.commands = cp -av meuh.jar -L $$LIBNAME $$ECLIPSE/LICS/lib

QMAKE_EXTRA_TARGETS  += meuh_wrap.cxx class meuh.jar eclipse


