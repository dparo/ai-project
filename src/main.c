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

    switch (tested->type) {
    case TT_PUNCT_BOTHDIR_ARROW: {
        result = true;
    } break;
    case TT_PUNCT_ARROW: {
        if (sample->type == TT_PUNCT_BOTHDIR_ARROW) {
            result = false;
        } else {
            result = true;
        }
    } break;

    case TT_PUNCT_LOGICAL_AND:
    case TT_PUNCT_BITWISE_AND: {
        if ( sample->type == TT_PUNCT_BOTHDIR_ARROW ||
             sample->type == TT_PUNCT_ARROW ) {
            result = false;
        } else {
            result = true;
        }
    } break;

    case TT_PUNCT_LOGICAL_OR:
    case TT_PUNCT_BITWISE_OR: {
        if ( sample->type == TT_PUNCT_BOTHDIR_ARROW ||
             sample->type == TT_PUNCT_ARROW ||
             sample->type == TT_PUNCT_LOGICAL_AND ||
             sample->type == TT_PUNCT_BITWISE_AND) {
            result = false;
        } else {
            result = true;
        }
        

    } break;

    case TT_PUNCT_LOGICAL_NOT:
    case TT_PUNCT_BITWISE_NOT: {
        if ( sample->type == TT_PUNCT_LOGICAL_NOT ||
             sample->type == TT_PUNCT_BITWISE_NOT ) {
            result = true;
        } else {
            result = false;
        }
    } break;
        
    default: {
        invalid_code_path();
    } break;
    }
    

    return result;
}




struct prop_calc_id {
    uint32_t id;
    uint32_t index;
};


void
bool_uint64_array_encoded_add( uint64_t *array,
                               size_t bits_count )
{
    assert(bits_count > 0);

    size_t nelems;
    if ( bits_count == 0 ) { nelems = 0; }
    else { nelems = ((bits_count - 1) / (sizeof(uint64_t) * 8)) + 1; }
   
    size_t bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(uint64_t) * 8); }

    for ( uint64_t *s = array; s < array + nelems; s ++) {
        // Needs to check for bubble up
        if ( s == (array + nelems - 1)) {
            uint64_t checkvalue = ((uint64_t) 0 - 1) >> ( sizeof(uint64_t) * 8 - bits_count_remainder);
                
            if ( *s == checkvalue ) {
                // Overflow simulation
                memset(array, 0, sizeof(array[nelems]));
                break;
            }
        }
        (*s)++;
        if ( s == 0 ) {
            // Bubble up the add
            continue;
        }
        break;
    }
}

void
test_uint64_t(void)
{
    uint64_t array[2];
    for ( size_t i = 1; i < 72; i++ ) {
        size_t bits_count = i;        
        bool_uint64_array_encoded_add(array, bits_count);
    }    
}

void
pcalc_encoded_preprocess ( stb_sdict *d,
                           uint64_t *array,
                           size_t bits_count )
{
    assert(bits_count > 0);

    size_t nelems;
    if ( bits_count == 0 ) { nelems = 0; }
    else { nelems = ((bits_count - 1) / (sizeof(uint64_t) * 8)) + 1; }
   
    size_t bits_count_remainder; (void) bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(uint64_t) * 8); }


    int it = 0;
    char *k;
    void *v;
    stb_sdict_for(d, it, k, v) {
        v = (void*) ((size_t)it);
        stb_sdict_set(d, k, v);
        printf("k : %s | v: %p\n", k, v);
    }

}


void
boolean_pack_into_uint64_array(bool value,
                               uint64_t *array,
                               size_t array_bits_count,
                               size_t bit_index )
{
    assert(bit_index < array_bits_count );
    size_t index = bit_index / ( sizeof(array[0]) * 8);
    size_t mask = array[index] & ~((size_t)1 << ((size_t)bit_index - index * sizeof(array[0]) * 8));
    array[index] = mask | (size_t)value << ((size_t)bit_index - index * sizeof(array[0]) * 8);
#if 0
    printf("bi: %zu | index: %zx | mask: %zx | array[index]: %zx\n",
           bit_index, index, mask, array[index]);
#endif

}



bool
boolean_unpack_from_uint64_array( uint64_t *array,
                                  size_t array_bits_count,
                                  size_t bit_index)
{
    assert(bit_index < array_bits_count );
    size_t index = bit_index / ( sizeof(array[0]) * 8);
    size_t local_bit_index = ((size_t)bit_index - index * sizeof(array[0]) * 8);
    size_t mask = ( (size_t) 1 << local_bit_index);
    bool result = (array[index] &  mask) >> local_bit_index;
#if 0
        printf("bi: %zu | index: %zx | local_bit_index: %zd | mask: %zx | result: %d\n",
               bit_index, index, local_bit_index, mask, result);
#endif

    return false;
}

void
boolean_packing_unpacking_test(void)
{
    uint64_t array[2] = {0};
    size_t array_bits_count = 128;
    bool value = 1;
    for (size_t i = 0; i < 100; i++ ) {
        value = !value;
        boolean_pack_into_uint64_array(value, array, array_bits_count, i);
    }
    for (size_t i = 0; i < 100; i++ ) {
        boolean_unpack_from_uint64_array(array, array_bits_count, i);
    }

}


bool
pcalc_encoded_compute_with_value(Token *queue,
                                 size_t queuesize,                                 
                                 stb_sdict *d,
                                 uint64_t *array,
                                 size_t bits_count )
{
    assert(bits_count > 0);

    size_t nelems;
    if ( bits_count == 0 ) { nelems = 0; }
    else { nelems = ((bits_count - 1) / (sizeof(uint64_t) * 8)) + 1; }
   
    size_t bits_count_remainder; (void) bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(uint64_t) * 8); }


    bool *stack = calloc(queuesize, 1);
    size_t stack_top = 0;

    if ( stack ) {

        for ( size_t it = 0; it < queuesize; it ++) {
            Token *t = & (queue[it]);
            if ( t->type == TT_IDENTIFIER ) {
                char temp = t->text[t->text_len];
                size_t index = (size_t) stb_sdict_get(d, t->text);
                t->text[t->text_len] = temp;
                
                stack[stack_top++] = array[index / sizeof(uint64_t)]
                    & ((uint64_t) - 1);

                assert_msg(0, "Needs proper packing and unpacking testing");
            } else {
                // needs to perform computation

                
            }
            
        }
        
        free(stack);
        return true;
    } else {
        return false;
    }

}

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

    size_t temp = 129/* stb_sdict_count(d) */;
    size_t allocationsize =
        ((temp - 1) / (sizeof(uint64_t) * 8)) * sizeof(uint64_t) + sizeof(uint64_t);

    assert(allocationsize);
    
    uint64_t *data = calloc(allocationsize, 1);

    if ( data ) {

        pcalc_encoded_preprocess(d, data, stb_sdict_count(d));
        size_t max_it = 1 << stb_sdict_count(d);

        for (size_t i = 0; i < max_it; i ++ ) {
#       if 0
            printf("%zx\n", data[0]);
#       endif
            // Use the value right here and compute
            {
                pcalc_encoded_compute_with_value(queue, queuesize, d, data, stb_sdict_count(d));
            }
            bool_uint64_array_encoded_add( data,
                                           stb_sdict_count(d) );
        }


        free(data);
    }
}


int main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);
#if 0
    pack_testing();
    return 0;
#endif
#if 0
    test_generator(); 
    return 0;
#endif


    char code [] =
        "( !A | B ) <-> ( C & !B  || !D | E | F| G)"
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
                        && ( pcalc_greater_or_eq_precedence(peek, & token)
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

