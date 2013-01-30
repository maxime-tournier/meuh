
TEMPLATE = lib
TARGET = lib/tool

CONFIG += qt
QT += xml gui opengl core

LIBS += -lqglviewer-qt4 -lcore -lmath -lphys  -lgeo -lcoll -lgui -lgl -lscript -lmocap

PKGCONFIG += lua5.1

HEADERS += \
#    anim.h \
    viewer.h \

SOURCES += \
    adaptor.cpp \
    mocap.cpp \
#    pga.cpp \
    bio/info.cpp \
    bio/nasa.cpp \
#    angular_limits.cpp \
#    character.cpp \
#    markers.cpp \
#    anim.cpp \
    model.cpp \
    rigid.cpp \
    picker.cpp \
#    sweeper.cpp \
    mesh.cpp \
#    trajectory.cpp \
    viewer.cpp \
    simulator.cpp \
	unilateral.cpp \
	contacts.cpp \
	friction.cpp \
	player.cpp \
	benchmark.cpp \
	asf.cpp \
	ragdoll.cpp \
	pga.cpp \
	control.cpp \
	to_lua.cpp \
	gl.cpp \
	ground.cpp \
	tweak.cpp \
	qp.cpp \