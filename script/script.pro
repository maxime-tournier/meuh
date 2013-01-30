

TEMPLATE = lib
# CONFIG += plugin

TARGET = lib/script

LIBS += -lcore -lreadline 
PKGCONFIG += luabind lua5.1

SOURCES += \
    lua.cpp \
    init.cpp \
    console.cpp \
    api.cpp \
    meuh.cpp \
    readline.cpp \
    history.cpp \
