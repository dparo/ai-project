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


#define EVAL_COMMANDLINE_INPLACE(intpt, command)                 \
    printf("\n\nformula: %s\n\n", command);                      \
    eval_commandline((intpt), strdup(command), strlen(command))
        

int
main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);

    char * commandline = NULL;
    size_t commandline_len;

    struct interpreter intpt = {0};
    
#if 0
    EVAL_COMMANDLINE_INPLACE ( & intpt, "(a(b & c) | d) + e(f)");
#else
    while ( 1 ) {
        if ( commandline ) { free(commandline); commandline_len = 0; }
        intpt_info_printf(& intpt, "\n\n");
        user_interact(& commandline, & commandline_len);
        if ( commandline && commandline_len) {
            printf("\n\n\n");
            eval_commandline( & intpt, commandline, commandline_len);
        } else {
            intpt_info_printf( &intpt, "Failed to get line from the terminal");
        }
    }
#endif
}


#endif /* MAIN_C_IMPL */

