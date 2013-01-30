TEMPLATE = lib
TARGET = lib/plugin.opencv 

# ubuntu might call this libcv-dev
deps.commands += libopencv-dev

PKGCONFIG += opencv
SOURCES += opencv.cpp image.cpp

