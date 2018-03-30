#!/bin/bash


gcc -D_GNU_SOURCE -D__DEBUG -std=gnu11 -O0 -g3 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -DMAIN_C_IMPL -I./ -I./__generated__ -I../common main.c -lm -ldl -pthread -o ./bin/http_chunked
