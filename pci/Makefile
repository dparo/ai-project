

BUILD_DEFS = -DMAIN_C_IMPL -D_GNU_SOURCE
WARNINGS = -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
INCLUDES = -I./ -I./ext-deps/stb -I./__generated__
LIBRARIES = -lm -lreadline -ldl -pthread
OUTPUT_PATH = ./bin/
OUTPUT = $(OUTPUT_PATH)pci


$(OUTPUT_PATH):
	mkdir -p $(OUTPUT_PATH)


bin/code-gen: $(OUTPUT_PATH)  code-gen/main.c
	gcc -g3 -D__DEBUG -DMAIN_C_IMPL code-gen/main.c -Iext-deps/stb/ -I./ -lm -o ./bin/code-gen
	./bin/code-gen

debug: $(OUTPUT_PATH) bin/code-gen
	gcc $(BUILD_DEFS) -D__DEBUG -std=gnu11 -O0 -g3 $(WARNINGS) $(INCLUDES) main.c $(LIBRARIES) -o $(OUTPUT)

release: $(OUTPUT_PATH)
	gcc $(BUILD_DEFS) -std=gnu11 -O3 $(WARNINGS) $(INCLUDES) main.c $(LIBRARIES) -o $(OUTPUT)

default: debug

all: debug

run: all
	$(OUTPUT)

run_terminal: all
	urxvt  -e "$(OUTPUT)"


generate_emacs_tagfile:
	etags $(shell find . -name "*.cpp" -print -or -name "*.h" -print -or -name "*.c" -print) --declarations &




