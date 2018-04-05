


debug:
	gcc -D_GNU_SOURCE -D__DEBUG -std=gnu11 -O0 -g3 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -DMAIN_C_IMPL -I./ -I./ext_deps/stb -I./__generated__ -I../common main.c -lm -lreadline -ldl -pthread -o ./bin/pci


release:
	gcc -D_GNU_SOURCE -std=gnu11 -O3 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -DMAIN_C_IMPL -I./ -I./ext_deps/stb -I./__generated__ -I../common main.c -lm -lreadline -ldl -pthread -o ./bin/pci


all: default

run: all
	./bin/pci

run_terminal: all
	urxvt  -e "./bin/pci"


generate_emacs_tagfile:
	etags $(shell find . -name "*.cpp" -print -or -name "*.h" -print -or -name "*.c" -print) --declarations &




