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



#define PROP_CALC_C_IMPL
#include "prop-calc.c"

#define PCALC_UTILS_C_IMPL
#include "pcalc_utils.c"


void log_token (Token *token)
{
     printf("%.*s\tline_number=%d\tcolumn=%d\n", token->text_len, token->text, token->line_num, token->column);
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
pcalc_symbol_table_preprocess_ids ( struct symbol_table *symtable )
{
    int it1 = 0, it2 = 0;
    char *k;
    void *v;
    stb_sdict_for(symtable->dict, it1, k, v) {
        v = (void*) ((size_t)it2);
        it2 ++ ;
        stb_sdict_set(symtable->dict, k, v);
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

bool
pcalc_encoded_compute_with_value(Token *queue,
                                 size_t queuesize,
                                 stb_sdict *d,
                                 uint64_t *array,
                                 size_t array_bits_count )
{
    assert(array_bits_count > 0);
    
    bool *stack = calloc(queuesize, 1);
    // assert_msg(0, "The stack should be provided from the caller");
    
    size_t stack_top = 0;
    if ( stack ) {
        for ( size_t it = 0; it < queuesize; it ++) {
            Token *t = & (queue[it]);
            if ( t->type == TT_IDENTIFIER ) {
                char temp = t->text[t->text_len];
                t->text[t->text_len] = 0;
                size_t bit_index = (size_t) stb_sdict_get(d, t->text);
                t->text[t->text_len] = temp;
                
                bool value = boolean_unpack_from_uint64_array( array,
                                                               array_bits_count,
                                                               bit_index );
                
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack

                
            }
            
        }
        
        free(stack);
        return true;
    } else {
        return false;
    }

}

void
pcalc_build_symbol_table_from_queue(struct ast_token_queue *queue,
                                    struct symbol_table *symtable)
{
    Token *t;
    size_t it;
    
    for ( it = 0, t = queue->tokens; it < queue->num_tokens;
          t = & (queue->tokens[++it])) {
        if ( t->type == TT_IDENTIFIER ) {
            //null terminate;
            symbol_table_add_identifier(symtable, t);
        }
    }   

}


size_t
ast_truth_table_packed_compute_required_size( size_t required_num_of_bits )
{
    assert(required_num_of_bits);
    return (required_num_of_bits - 1) /
        (sizeof(ast_truth_value_packed) * 8) * sizeof(ast_truth_value_packed) + sizeof(ast_truth_value_packed);
}

struct ast_truth_table_packed*
ast_truth_table_packed_alloc_from_symtable(struct symbol_table *symtable)
{
    struct ast_truth_table_packed* result;

    size_t allocationsize =
        ast_truth_table_packed_compute_required_size(symbol_table_num_ids(symtable));
    assert(allocationsize);

    result = calloc(allocationsize, 1);
    assert(result);

    result->num_bits = symbol_table_num_ids(symtable);
    /* allocates some uint64_t based on the count and encode */
    /*     all possible booleans in there; */

    


    return result;
}

void
bruteforce_solve(struct ast_token_queue *queue)
{
    struct symbol_table symtable = symbol_table_new();


    pcalc_build_symbol_table_from_queue(queue, &symtable);
    pcalc_symbol_table_preprocess_ids(& symtable);
    
    struct ast_truth_table_packed *ast_ttp =
        ast_truth_table_packed_alloc_from_symtable(& symtable);

    if ( ast_ttp ) {

        size_t max_it = 1 << symbol_table_num_ids(& symtable);

        for (size_t i = 0; i < max_it; i ++ ) {
#       if 0
            printf("%zx\n", data[0]);
#       endif
            // Use the value right here and compute
            {
#           if 0
                pcalc_encoded_compute_with_value(queue->tokens, queue->num_tokens, d, data, stb_sdict_count(d));
#           endif
                assert_msg(0, "Need to refactor above line and function");
            }

#       if 0
            bool_uint64_array_encoded_add( data,
                                           stb_sdict_count(d) );
#       endif
            assert_msg(0, "Need to refactor above line and function");
        }


        free(ast_ttp);
    }
}


#define TEST_C_IMPL
#include "test.c"

int main( int argc, char **argv)
{
    platform_init();
    UNUSED(argc), UNUSED(argv);
    char code [] =
        "( !A | B ) <-> ( C & !B  || !D | E | F| G)"
        "\0\0\0\0\0\0";
    
    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, code,
                                sizeof(code),
                                "*code*");

    // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

    // operator stack
    struct ast_token_stack stack = {0};
    struct ast_token_queue queue = {0};
    
    
    

     
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
                ast_token_queue_push(&queue, &token);
            } /* else if ( is function ) */
            else if ( prop_calc_token_is_operator(& token )) {
                Token *peek = NULL;
                while ( ( (stack.num_tokens) != 0 && (peek = ast_token_stack_peek_addr(&stack)))
                        && ( pcalc_greater_or_eq_precedence(peek, & token)
                             && peek->type != TT_PUNCT_OPEN_PAREN)) {
                    ast_token_queue_push( &queue, peek);
                    ast_token_stack_pop( & stack);
                }
                ast_token_stack_push( & stack, & token);
            } else if ( token.type == TT_PUNCT_OPEN_PAREN ) {
                ast_token_stack_push( & stack, & token);
            } else if (token.type == TT_PUNCT_CLOSE_PAREN ) {
                Token *peek = NULL;
                while ( ( (stack.num_tokens) != 0 && (peek = ast_token_stack_peek_addr(&stack)))
                        && ( peek->type != TT_PUNCT_OPEN_PAREN)) {
                    ast_token_queue_push( &queue, peek);
                    ast_token_stack_pop( & stack);
                }
                if ( stack.num_tokens == 0 ) {
                    if ( peek && peek->type != TT_PUNCT_OPEN_PAREN ) {
                        // Mismatched parentheses
                        fprintf(stderr, "Mismatched parens\n");
                        goto parse_end;
                    }
                } else {
                    // pop the closed paren from the stack
                    ast_token_stack_pop( & stack);
                }
            }
        }
        

    }

    /* if there are no more tokens to read: */
    /* 	while there are still operator tokens on the stack: */
    /* 		/\* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. *\/ */
    /* 		pop the operator from the operator stack onto the output queue. */

    Token *peek = NULL;
    while ( ( (stack.num_tokens) != 0 && (peek = ast_token_stack_peek_addr(&stack)))) {
        if ( peek->type == TT_PUNCT_OPEN_PAREN ||
             peek->type == TT_PUNCT_CLOSE_PAREN ) {
            fprintf(stderr, "Mismatched parens\n");
            goto parse_end;
        }
        ast_token_queue_push( & queue, peek);
        ast_token_stack_pop ( & stack );
    }

    
parse_end: {
    }
    
    for ( size_t i = 0; i < queue.num_tokens; i++ ) {
        log_token(& (queue.tokens[i]));
    }


    printf("Running bruteforce method\n");

    bruteforce_solve(& queue);
    
    
    puts("Exiting application");
    return 0;

}





#endif /* MAIN_C_IMPL */

