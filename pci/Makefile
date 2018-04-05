

BUILD_DEFS = -DMAIN_C_IMPL -D_GNU_SOURCE
WARNINGS = -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
INCLUDES = -I./ -I./ext_deps/stb -I./__generated__
LIBRARIES = -lm -lreadline -ldl -pthread
OUTPUT = ./bin/pci

debug:
	gcc $(BUILD_DEFS) -D__DEBUG -std=gnu11 -O0 -g3 $(WARNINGS) $(INCLUDES) main.c $(LIBRARIES) -o $(OUTPUT)


release:
	gcc $(BUILD_DEFS) -std=gnu11 -O3 $(WARNINGS) $(INCLUDES) main.c $(LIBRARIES) -o $(OUTPUT)

default: debug

all: debug

run: all
	$(OUTPUT)

run_terminal: all
	urxvt  -e "$(OUTPUT)"


generate_emacs_tagfile:
	etags $(shell find . -name "*.cpp" -print -or -name "*.h" -print -or -name "*.c" -print) --declarations &




