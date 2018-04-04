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
user_interact(char **formula, size_t *formula_size)
{
    // Maybe continue asking for more input if last character is like `\`
    // in a loop and concatenate to previous string
    char *string = readline ("Input formula to compute:\n    [>] ");
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
        *formula = string;
        *formula_size = size;
    }

    if (string && string[0] != '\0') {
        add_history(string); // NOTE: Returns void
    }
}


int
main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);


    char input_line[4096];
    size_t input_line_len = 0;
    enum { EXTRA_SPACE_FOR_NULL_TERMINATION = 5};
    char *readres = NULL;
    	
    char *formula = NULL;
    size_t formula_size;


    struct ast ast = {0};
    
    while ( 1 ) {
        if ( formula ) { free(formula); formula_size = 0; }
        printf("\n\n");
        user_interact(& formula, & formula_size);
        if ( formula ) {
            printf("\n\n\n");
            
            build_ast( &ast, formula, formula_size );
            eval_ast( &ast);
        }
    }
}


#endif /* MAIN_C_IMPL */

