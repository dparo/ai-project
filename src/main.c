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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-value"
#define STB_DEFINE
#include "stb.h"
#pragma GCC diagnostic pop


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


bool
prop_calc_token_is_operator(Token *t)
{
    bool result = true;
    if (   t->type == TT_NONE
        || t->type == TT_IDENTIFIER
        || t->type == TT_PUNCT_OPEN_PAREN
        || t->type == TT_PUNCT_CLOSE_PAREN
        || t->type == TT_CONSTANT ) {
        result = false;
    }
    return result;
}


bool
pcalc_greater_or_eq_precedence(Token *sample,
                               Token *tested )
{
    bool result = false;

    return result;
}




struct prop_calc_id {
    uint32_t id;
    uint32_t index;
};

void
bruteforce_solve(Token *queue,
                 size_t queuesize)
{
    stb_sdict *d = stb_sdict_new(1);

    
    
    Token *t = queue;
    size_t it = 0;
    for ( t = queue; it < queuesize; t = & queue[++it]) {
        if ( t->type == TT_IDENTIFIER ) {
            char temp = t->text[t->text_len];
            t->text[t->text_len] = 0;
            stb_sdict_set(d, t->text, NULL);
            t->text[t->text_len] = temp;
        }
    }   

    int i = 0;
    char *k;
    void *v;


    /* allocates some uint64_t based on the count and encode */
    /*     all possible booleans in there; */
    
    for ( int c = 0; c < stb_sdict_count(d); c ++ ) {
        for (int it = 0; it < c || it == 0; it ++ ) {
            for( int k = 0; k < 2; k++) {
                
            }
        }
    }

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
        "( !A | B ) <-> ( C & !B )"
        "\0\0\0\0\0\0";
    
    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, code,
                                sizeof(code),
                                "*code*");

    Token function_name = Empty_Token;
    bool searching_for_function = true;
     
    i32 nested_parens = 0;
    i32 nested_brackets = 0;
    i32 nested_braces = 0;




    // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

    // operator stack
    Token stack[1024];
    size_t stack_top = 0;


    // output queue
    Token queue[1024];
    size_t queue_top = 0;
    
    
#define TOKEN_STACK_PUSH( token )                                       \
    do { (stack)[(stack_top)++] = (token); assert((stack_top) != sizeof(stack)); } while(0)
    
#define TOKEN_QUEUE_PUSH( token )                                       \
    do { (queue)[(queue_top)++] = (token); assert((queue_top) != sizeof(queue)); } while(0)

     
    // NOTE: Maybe better error handling because even the push_state can throw an error
    //       Maybe set a locked variable inside the tokenizer for critical stuff
    //       that will inevitably inject more complexity on the library side
    while (!done && get_next_token( &tknzr, & token)) {
        // NOTE: At every iteration the tokenizer error is cleared with the call to get_next_token
        if ( tknzr.err ) {
            puts(tknzr.err_desc);
            assert_msg(0, "We got an error boys");
        }    

        //log_token(& token);

        {
            if (token.type == TT_CONSTANT ||
                token.type == TT_IDENTIFIER ) {
                TOKEN_QUEUE_PUSH(token);
            } /* else if ( is function ) */
            else if ( prop_calc_token_is_operator(& token )) {
                Token *peek = NULL;
                while ( ( stack_top != 0 && (peek = & (stack[stack_top - 1])))
                        && ( pcalc_greater_or_eq_precedence(& token, peek)
                             && peek->type != TT_PUNCT_OPEN_PAREN)) {
                    TOKEN_QUEUE_PUSH(*peek);
                    stack_top--;
                }

                TOKEN_STACK_PUSH(token);
            } else if ( token.type == TT_PUNCT_OPEN_PAREN ) {
                TOKEN_STACK_PUSH(token);
            } else if (token.type == TT_PUNCT_CLOSE_PAREN ) {
                Token *peek = NULL;
                while ( ( stack_top != 0 && (peek = & (stack[stack_top - 1])))
                        && ( peek->type != TT_PUNCT_OPEN_PAREN)) {
                    TOKEN_QUEUE_PUSH(*peek);
                    stack_top--;
                }
                if ( stack_top == 0 ) {
                    if ( peek && peek->type != TT_PUNCT_OPEN_PAREN ) {
                        // Mismatched parentheses
                        fprintf(stderr, "Mismatched parens\n");
                        goto parse_end;
                    }
                } else {
                    // pop the closed paren from the stack
                    stack_top--;
                }
            }
        }
        

    }

    /* if there are no more tokens to read: */
    /* 	while there are still operator tokens on the stack: */
    /* 		/\* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. *\/ */
    /* 		pop the operator from the operator stack onto the output queue. */

    Token *peek = NULL;
    while ( stack_top != 0 && (peek = & (stack[stack_top - 1]))) {
        if ( peek->type == TT_PUNCT_OPEN_PAREN ||
             peek->type == TT_PUNCT_CLOSE_PAREN ) {
            fprintf(stderr, "Mismatched parens\n");
            goto parse_end;
        }
        TOKEN_QUEUE_PUSH(*peek);
        stack_top--;
    }

    
parse_end: {
    }
    
    for ( size_t i = 0; i < queue_top; i++ ) {
        log_token(& (queue[i]));
    }


    printf("Running bruteforce method\n");

    bruteforce_solve(queue, queue_top);
    
    
    puts("Exiting application");
    return 0;

}





#endif /* MAIN_C_IMPL */

