
fresh.commands = rm lib -rf; rm bin -rf ; rm -v -rf `find -name '*.gch'`
QMAKE_EXTRA_TARGETS += fresh env 
