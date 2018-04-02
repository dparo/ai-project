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
pcalc_symbol_table_preprocess_ids ( struct symbol_table *symtable )
{
    int it1 = 0, it2 = 0;
    char *k;
    void *v;
    stb_sdict_for(symtable->dict, it1, k, v) {
        v = (void*) ((size_t)it2);
        it2 ++ ;
        stb_sdict_set(symtable->dict, k, v);
        printf("k : %s | v: %zu\n", k, (size_t) v);
    }
}

void
pcalc_perform_operation_from_queue( Token *t,
                                    struct ast_computation_stack *stack)
{
    bool result = 0;
    bool v1 = 0;
    bool v2 = 0;

#define CHECK_1OPERANDS(stack) do {                  \
        if (! (((stack).num_bools) >= 1 )) {    \
            goto not_enough_operands;           \
        } } while (0)

#define CHECK_2OPERANDS(stack) do {             \
        if (!( ((stack).num_bools) >= 2 )) {    \
            goto not_enough_operands;           \
        } } while (0)
   
    switch (t->type) {
    case TT_PUNCT_BOTHDIR_ARROW: {
        CHECK_2OPERANDS(*stack);
        v2 = ast_computation_stack_pop_value(stack);
        v1 = ast_computation_stack_pop_value(stack);
        result = (v1 && v2) || !((v1 || v2));
        ast_computation_stack_push(stack, result);
    } break;
    case TT_PUNCT_ARROW: {
        CHECK_2OPERANDS(*stack);
        v2 = ast_computation_stack_pop_value(stack);
        v1 = ast_computation_stack_pop_value(stack);
        result = true;
        if ( v1 == true && v2 == false ) { result = false;}
        ast_computation_stack_push(stack, result);
    } break;

    case TT_PUNCT_LOGICAL_AND:
    case TT_PUNCT_BITWISE_AND: {
        CHECK_2OPERANDS(*stack);
        v2 = ast_computation_stack_pop_value(stack);
        v1 = ast_computation_stack_pop_value(stack);
        result = v1 && v2;
        ast_computation_stack_push(stack, result);
    } break;

    case TT_PUNCT_LOGICAL_OR:
    case TT_PUNCT_BITWISE_OR: {
        CHECK_2OPERANDS(*stack);
        v2 = ast_computation_stack_pop_value(stack);
        v1 = ast_computation_stack_pop_value(stack);
        result = v1 || v2;
        ast_computation_stack_push(stack, result);
    } break;

    case TT_PUNCT_LOGICAL_NOT:
    case TT_PUNCT_BITWISE_NOT: {
        CHECK_1OPERANDS(*stack);
        v1 = ast_computation_stack_pop_value(stack);
        result = !v1;
        ast_computation_stack_push(stack, result);
    } break;
        

    default: {
        assert_msg(0, "Operator is not supported");
    } break;
    }
    
    return;
not_enough_operands: {
        assert_msg(0, "Operator needs more operand, not found enough inside the stack");
        return;
    }

#undef CHECK_1OPERANDS
#undef CHECK_2OPERANDS
}

void
pcalc_encoded_compute_with_value( struct ast_token_queue *queue,
                                  struct symbol_table *symtable,
                                  struct ast_truth_table_packed *ast_ttp )
{
    assert(ast_ttp->num_bits > 0);

    struct ast_computation_stack stack;
    stack.num_bools = 0;

    // In the future this if will check for valid allocation
    if ( stack.bools ) {
        Token *t;
        size_t it;
    
        ast_token_queue_for(it, *queue, t) {
            if ( t->type == TT_IDENTIFIER ) {
                size_t bit_index = symbol_table_get_identifier_value(symtable, t);

                bool value = ast_truth_table_unpack_bool( ast_ttp,
                                                          bit_index);
                // printf(" bit_index: %zx | unpacked bool: %d\n", bit_index, value);
                ast_computation_stack_push( & stack, value );
                
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack
                pcalc_perform_operation_from_queue( t, & stack ) ;
                printf("### Result: %d\n", ast_computation_stack_peek_value(& stack));
            }
            
        }
        assert_msg(stack.num_bools == 1, "Stack should remain with 1 value only");
        printf("### Final Result: %d\n", ast_computation_stack_pop_value(& stack));
    }
}

void
pcalc_build_symbol_table_from_queue(struct ast_token_queue *queue,
                                    struct symbol_table *symtable)
{
    Token *t;
    size_t it;
    
    ast_token_queue_for(it, *queue, t) {
        if ( t->type == TT_IDENTIFIER ) {
            //null terminate;
            symbol_table_add_identifier(symtable, t);
        }
    }
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
#       if 1
            ast_truth_table_packed_dbglog(ast_ttp);
#       endif
            // Use the value right here and compute
            {
                pcalc_encoded_compute_with_value(queue, & symtable, ast_ttp );
            }

            ast_truth_table_packed_generate_next_combination(ast_ttp );
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
        "(!A && B ) || C && (G <-> D)"
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

    Token *t;
    size_t it;
    
    ast_token_queue_for(it, queue, t) {
        log_token(t);
    }

    printf("Running bruteforce method\n");

    bruteforce_solve(& queue);
    
    
    puts("Exiting application");
    return 0;

}





#endif /* MAIN_C_IMPL */

