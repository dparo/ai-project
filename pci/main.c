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
#include <math.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>


#define UNUSED(...) (void)(__VA_ARGS__)
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>



#define PARSER_C_IMPL
#include "parser.c"



#define LANGUAGE_C_IMPL
#include "language.c"


#define INTERPRETER_C_IMPL
#include "interpreter.c"


#include <readline/readline.h>
#include <readline/history.h>


// the formula is null terminated with necessary space for null termination
void
user_interact(char **commandline, size_t *commandline_size)
{
    assert(commandline);
    assert(commandline_size);
    // Maybe continue asking for more input if last character is like `\`
    // in a loop and concatenate to previous string
    char *string = readline ("Input Interpreter Command:\n    [>] ");
    enum { EXTRA_SPACE_FOR_NULL_TERMINATION = 5};
    size_t len = 0;
    size_t size = 0;
    if ( string ) {
        len = strlen(string);
        size = len + EXTRA_SPACE_FOR_NULL_TERMINATION;
        void * temp = realloc(string, size);
        if ( temp ) { string = temp; }
        else { fprintf(stderr, "Failed Reading user input\n"); }
    } else { fprintf(stderr, "Failed Reading user input\n"); }

    if ( string ) {
        memset(string + len, 0, EXTRA_SPACE_FOR_NULL_TERMINATION);
        *commandline = string;
        *commandline_size = size;
    }

    if (string && string[0] != '\0') {
        add_history(string); // NOTE: Returns void
    }
}


#define EVAL_COMMANDLINE_INPLACE(intpt, command)                 \
    printf("\n\nformula: %s\n\n", command);                      \
    eval_commandline((intpt), strndup(command, sizeof(command)), \
                     sizeof(command))
        

int
main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);


    char * commandline = NULL;
    size_t commandline_size;


    struct interpreter intpt = {0};

#if 1
    EVAL_COMMANDLINE_INPLACE ( & intpt, " a == b ? c : 0"//
                               "\0\0\0\0\0\0\0" );
#else
    while ( 1 ) {
        if ( commandline ) { free(commandline); commandline_size = 0; }
        printf("\n\n");
        user_interact(& commandline, & commandline_size);
        if ( commandline ) {
            printf("\n\n\n");
            eval_commandline( & intpt, commandline, commandline_size);
        }
    }
#endif
}


#endif /* MAIN_C_IMPL */

