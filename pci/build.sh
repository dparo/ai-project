#!/bin/bash

BUILD_DEFS="-DMAIN_C_IMPL -D_GNU_SOURCE"
WARNINGS="-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable"
INCLUDES="-I./ -I./ext-deps/stb -I./__generated__"
LIBRARIES="-lm -lreadline -ldl -pthread"
OUTPUT_PATH="./bin"
OUTPUT="$OUTPUT_PATH/pci"



if [ "$1" = "code-gen" ]; then
    set -x
    gcc -g3 -std=gnu11 -D_GNU_SOURCE -D__DEBUG -DMAIN_C_IMPL code-gen/main.c -Iext-deps/stb/ -I./ -lm -o ./bin/code-gen
    ./bin/code-gen
elif [ "$1" = "debug" ]; then
    set -x
    gcc $BUILD_DEFS -D_GNU_SOURCE -D__DEBUG -std=gnu11 -O0 -g3 $WARNINGS $INCLUDES main.c $LIBRARIES -o $OUTPUT
elif [ "$1" = "release" ]; then
    set -x
    gcc $BUILD_DEFS -D_GNU_SOURCE -std=gnu11 -O3 $WARNINGS $INCLUDES main.c $LIBRARIES -o $OUTPUT
fi
