/*
 * Copyright (C) 2018  Davide Paro
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MAIN_C_INCLUDE
#define MAIN_C_INCLUDE


//#######################################################
#endif /* MAIN_C_INCLUDE */
#if !defined MAIN_C_IMPLEMENTED && defined MAIN_C_IMPL
#define MAIN_C_IMPLEMENTED
//#######################################################



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>


#define     UTILS_C_IMPL
#include   "utils.c"




#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"




static void
fatal(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(-1);
}


#define     PLATFORM_C_IMPL
#include   "platform.c"




#include <math.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>


#define UNUSED(...)  (void)(__VA_ARGS__)
#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#define MAX(a,b)     ((a) < (b) ? (b) : (a))





#define MEMORY_C_IMPL
#include "memory.c"


#define TIMING_C_IMPL
#include "timing.c"


#define PARSER_C_IMPL
#include "parser.c"


#define LANGUAGE_C_IMPL
#include "language.c"

#define MEM_LAYOUT_C_IMPL
#include "mem-layout.c"


#include "ast.h"
#include "ast_node_stack.h"

#define INTERPRETER_UTILS_C_IMPL
#include "interpreter-utils.c"



#define AST_INDEXING_C_IMPL
#include "ast-indexing.c"

#define LISTS_C_IMPL
#include "lists.c"

#define INTERPRETER_C_IMPL
#include "interpreter.c"


#include <readline/readline.h>
#include <readline/history.h>


void
user_interact(char **commandline, size_t *commandline_len)
{
    assert(commandline);
    assert(commandline_len);
    // Maybe continue asking for more input if last character is like `\`
    // in a loop and concatenate to previous string
    char *string = readline( ANSI_COLOR_CYAN "Input Interpreter Command:" ANSI_COLOR_RESET
                             "\n   " ANSI_COLOR_CYAN "[>]" ANSI_COLOR_RESET " ");
    size_t len = 0;
    if ( string ) {
        len = strlen(string);
    } else { fprintf(stderr, "Failed Reading user input\n"); }

    if ( string ) {
        *commandline = string;
        *commandline_len = len;
    }

    if (string && string[0] != '\0') {
        add_history(string); // NOTE: Returns void
    }
}


#define EVAL_COMMANDLINE_INPLACE(command)                   \
    do {                                                    \
        printf("\n\nformula: %s\n\n", command);             \
        char *string = strdup(command);                     \
        eval_commandline(string, strlen(command));          \
        free(string);                                       \
    } while(0)


#define COMM_LINE_ARGS_C_IMPL
#include "comm-line-args.c"

int
main( int argc, char **argv)
{
#if defined __DEBUG
    printf("@@@ NOTE: DEBUG BUILD @@@\n");
#endif
    UNUSED(argc), UNUSED(argv);

    platform_init();
    interpreter_init();
    comm_line_args(argc, argv);

    char * commandline = NULL;
    size_t commandline_len = 0;

#if 0
    // Turn this branching on for quick testing of a specific formula,
    // Fast easy debugging
    EVAL_COMMANDLINE_INPLACE("a == b ^ c -> d | e <-> f ^ g & h | j | k & l > n | m & o ^ q");
#else
    while ( 1 ) {
        if ( commandline ) { free(commandline); commandline_len = 0; }
        interpreter_logi("\n\n");
        user_interact(& commandline, & commandline_len);
        if ( commandline && commandline_len) {
            printf("\n\n\n");
            eval_commandline( commandline, commandline_len);
        } else {
            interpreter_logi("Failed to get line from the terminal");
        }
    }
#endif

    printf("\n\n###################################################\n");
    printf("###########  TERMINATING APPLICATION ##############\n");
    printf("###################################################\n\n");


    interpreter_terminate();
    
    return 0;
}


#endif /* MAIN_C_IMPL */

