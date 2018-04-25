#ifndef MAIN_C_INCLUDE
#define MAIN_C_INCLUDE


//#######################################################
#endif /* MAIN_C_INCLUDE */
#if !defined MAIN_C_IMPLEMENTED && defined MAIN_C_IMPL
#define MAIN_C_IMPLEMENTED
//#######################################################

#define     UTILS_C_IMPL
#include   "utils.c"

#define     PLATFORM_C_IMPL
#include   "platform.c"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>


#define UNUSED(...)  (void)(__VA_ARGS__)
#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#define MAX(a,b)     ((a) < (b) ? (b) : (a))


#define MEMORY_C_IMPL
#include "memory.c"

#define PARSER_C_IMPL
#include "parser.c"



#define LANGUAGE_C_IMPL
#include "language.c"

#define MEM_LAYOUT_C_IMPL
#include "mem-layout.c"

#define INTERPRETER_C_IMPL
#include "interpreter.c"


#include <readline/readline.h>
#include <readline/history.h>


// the formula is null terminated with necessary space for null termination
void
user_interact(char **commandline, size_t *commandline_len)
{
    assert(commandline);
    assert(commandline_len);
    // Maybe continue asking for more input if last character is like `\`
    // in a loop and concatenate to previous string
    char *string = readline("\001\033[1;32m\002Input Interpreter Command:\n    [>]\001\033[0m\002 ");
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


#define EVAL_COMMANDLINE_INPLACE(command)              \
    printf("\n\nformula: %s\n\n", command);            \
    eval_commandline(strdup(command), strlen(command))
        

#define COMM_LINE_ARGS_C_IMPL
#include "comm-line-args.c"

int
main( int argc, char **argv)
{
    comm_line_args(argc, argv);
    platform_init();
    UNUSED(argc), UNUSED(argv);

    char * commandline = NULL;
    size_t commandline_len = 0;

    interpreter_init();
   
#if 0
    EVAL_COMMANDLINE_INPLACE ("~~a == ~b");
    // EVAL_COMMANDLINE_INPLACE ("(a ~| b)");
    //EVAL_COMMANDLINE_INPLACE ("(a <-> b) == (a == b)");
    //EVAL_COMMANDLINE_INPLACE ("a = {b, c & d, e}");
    //EVAL_COMMANDLINE_INPLACE ("(#x a) & b");
    //EVAL_COMMANDLINE_INPLACE ("a & (#x b)");
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

