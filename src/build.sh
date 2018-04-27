#!/bin/bash

BUILD_DEFS="-DMAIN_C_IMPL -D_GNU_SOURCE"
WARNINGS="-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable"
INCLUDES="-I./ -I./ext-deps/stb -I./__generated__ -I./common"
LIBRARIES="-lm -lrt -lreadline -ldl -pthread"
OUTPUT_PATH="../bin"



if [ "$1" = "code-gen" ]; then
    set -x
    rm __generated__/*
    gcc -g3 -std=gnu11  $BUILD_DEFS -D__DEBUG code-gen/main.c $INCLUDES -I./ -lm -o $OUTPUT_PATH/code-gen && ../bin/code-gen
elif [ "$1" = "debug" ]; then
    set -x
    gcc $BUILD_DEFS -D__DEBUG -std=gnu11 -O0 -g3 $WARNINGS $INCLUDES sat/main.c $LIBRARIES -o $OUTPUT_PATH/pci
elif [ "$1" = "release" ]; then
    set -x
    gcc $BUILD_DEFS -std=gnu11 -O3 $WARNINGS $INCLUDES sat/main.c $LIBRARIES -o $OUTPUT_PATH/pci
fi
