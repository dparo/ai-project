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


static void
fatal(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(-1);
}


#define     UTILS_C_IMPL
#include   "utils.c"

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

#define INTERPRETER_C_IMPL
#include "interpreter.c"


#include <readline/readline.h>
#include <readline/history.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// the formula is null terminated with necessary space for null termination
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
    UNUSED(argc), UNUSED(argv);

    platform_init();
    interpreter_init();
    comm_line_args(argc, argv);



    char * commandline = NULL;
    size_t commandline_len = 0;



    // @NOTE: This theorem prover: ~(a & b) == (~a | ~b) works
    //        This does not  --->  ~(a | b) == (~a & ~b)
   
#if 0


    
    EVAL_COMMANDLINE_INPLACE("(a <-> b) == ~(a ^ b)");

    //EVAL_COMMANDLINE_INPLACE("~(a | b) == (~a & ~b)");
    //EVAL_COMMANDLINE_INPLACE("~(a & b) == (~a | ~b)");


    //EVAL_COMMANDLINE_INPLACE ("a & 0");
    //EVAL_COMMANDLINE_INPLACE ("0 & 0");
    //EVAL_COMMANDLINE_INPLACE ("(~(a | b)) == (~a & ~b)");
    //EVAL_COMMANDLINE_INPLACE ("0 | 0");
    //EVAL_COMMANDLINE_INPLACE ("1 | 0");
    //EVAL_COMMANDLINE_INPLACE ("0 | 1");
    
    //EVAL_COMMANDLINE_INPLACE ("a = f | (b, c)");

    //EVAL_COMMANDLINE_INPLACE ("a = f(b, c)");


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

