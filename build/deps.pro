
LUA_PACKAGES = liblua5.1-0-dev libluabind-dev luarocks # liblua5.1-filesystem-dev

QT_PACKAGES = libqt4-dev libqglviewer-qt4-dev

JAVA_PACKAGES = swig2.0 openjdk-6-jdk

PACKAGES = pkg-config libboost-all-dev g++-4.6 libeigen3-dev libglew-dev $$LUA_PACKAGES $$QT_PACKAGES $$JAVA_PACKAGES

PACKAGES += libqglviewer-qt4-2 libglew-dev

# install needed software 
deps.commands =  sudo apt-get install $$PACKAGES

QMAKE_EXTRA_TARGETS += deps