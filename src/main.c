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



# define YIELD_NORES()                                                   \
    do { s.j = && jmp##__func__##__LINE__; return; jmp##__func__##__LINE__: {} } while(0)

# define YIELD(result)                                                  \
    do { s.j = && jmp##__func__##__LINE__; return (result); jmp##__func__##__LINE__: {} } while(0)


int
coro_enginnering(void)
{
    static struct {
        void *j;
        
        int a;
        int b;
    } s = {};

    if (s.j) {
        goto *(s.j);
    }

    

    
    memset(&s, 0, sizeof(s));

    s.a = 10;
    s.b = 11;


    YIELD(10);
       
    s.j = NULL;
    return 0;
    
}

int generator(void)
{
    static struct {
        void *j;

        
        int a;
    } s = {0};


    if (s.j) {
        goto *(s.j);
    } else {
        memset(&s, 0, sizeof(s));
    }



    for ( s.a = 0; s.a < 10; s.a++ ) {
        YIELD(s.a);
    }
    

    
    s.j = NULL;    //return 0;
    return -1;

}



#define PROP_CALC_C_IMPL
#include "prop-calc.c"

void test_generator(void)
{

    int value = 0;
    for ( int it = 0; it < 3; it++ ) {
        while (value = generator(), value != -1 ) {
            printf("iteration %d -> %d\n", it, value);
        }
    }
}


void log_token (Token *token)
{
     printf("%.*s\tline_number=%d\tcolumn=%d\n", token->text_len, token->text, token->line_num, token->column);
}


bool
string_starts_with(char *starter,
                   char *string,
                   i32 string_len )
{
     bool result = true;
     i32 i;
     for ( i = 0;
           (starter[i] != 0) && (i < string_len);
           ++i ) {
          if ( starter[i] != string[i] ) {
               result = false;
               break;
          }
     }

     return result;
}

 
int main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);
#if 0
    test_generator();
    return 0;
#endif



    char code [] =
        "( A | B ) <-> ( C && B )"
        "\0\0\0\0\0\0";
    
    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, code,
                                sizeof(code),
                                "");

    Token function_name = Empty_Token;
    bool searching_for_function = true;
     
    i32 nested_parens = 0;
    i32 nested_brackets = 0;
    i32 nested_braces = 0;

     
     
     
    // NOTE: Maybe better error handling because even the push_state can throw an error
    //       Maybe set a locked variable inside the tokenizer for critical stuff
    //       that will inevitably inject more complexity on the library side
    while (!done && get_next_token( &tknzr, & token)) {
        // NOTE: At every iteration the tokenizer error is cleared with the call to get_next_token
        if ( tknzr.err ) {
            assert_msg(0, "We got an error boys");
            puts(tknzr.err_desc);
        }
        log_token(& token);
    }
    puts("Exiting application");
    return 0;

}





#endif /* MAIN_C_IMPL */

