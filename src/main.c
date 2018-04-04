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
token_is_operator(Token *t)
{
    bool result = true;
    if ( t->type > TT_PUNCT_ENUM_OPERATORS_START_MARKER &&
         t->type < TT_PUNCT_ENUM_OPERATORS_END_MARKER ) {
        result = true;
    } else {
        result = false;
    }
    return result;
}
enum operator_associativity {
    NON_ASSOCIATIVE,
    RIGHT_ASSOCIATIVE,
    LEFT_ASSOCIATIVE,
};

static struct operator_infos {
    int precedence;
    uint numofoperands;
    enum operator_associativity associativity;
} ops[] = {
    [TT_PUNCT_SEMICOLON] = { 200, 1, RIGHT_ASSOCIATIVE },
    [TT_PUNCT_COMMA] = { 100, 2, LEFT_ASSOCIATIVE },
    
    [TT_PUNCT_BOTHDIR_ARROW] = { 5, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_ARROW] = { 5, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_LOGICAL_AND] = { 4, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_BITWISE_AND] = { 4, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_LOGICAL_OR] = { 3, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_BITWISE_OR] = { 3, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_LOGICAL_NOT] = { 2, 1, LEFT_ASSOCIATIVE },
    [TT_PUNCT_BITWISE_NOT] = { 2, 1, LEFT_ASSOCIATIVE },
    [TT_PUNCT_EQUAL] = { 1, 2, LEFT_ASSOCIATIVE },
    [TT_PUNCT_EQUAL_EQUAL] = { 1, 2, LEFT_ASSOCIATIVE },
    
    
    // Not valid set of types operators.
    [0 ... TT_PUNCT_ENUM_OPERATORS_START_MARKER] = { -1, 0, LEFT_ASSOCIATIVE },
    [TT_PUNCT_ENUM_MARKER_NOT_IMPLEMENTED_OPERATORS ... TT_PUNCT_ENUM_LAST_VALUE ] = { -1, 0, LEFT_ASSOCIATIVE },
};

int
operator_precedence(Token *t)
{
    assert(token_is_operator(t));
    return ops[t->type].precedence;
}

bool
op_is_left_associative(Token *t)
{
    return (ops[t->type].associativity == LEFT_ASSOCIATIVE);
}

bool
op_greater_precedence(Token *sample,
                   Token *tested )
{
    int p1 = operator_precedence(sample);
    int p2 = operator_precedence(tested);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 < p2;
}

bool
op_eq_precedence(Token *sample,
                 Token *tested )
{
    int p1 = operator_precedence(sample);
    int p2 = operator_precedence(tested);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 == p2;
}




void
symbol_table_preprocess_ids ( struct symbol_table *symtable )
{

    int it1 = 0, it2 = 0;
    char *k;
    void *v;
    ast_symbol_table_for(it1, symtable, k, v) {
        v = (void*) ((size_t)it2);
        it2 ++ ;
        stb_sdict_set(symtable->dict, k, v);
        //printf("k : %s | v: %zu\n", k, (size_t) v);
    }
}



uint
operator_numofoperands(Token *t)
{
    assert(token_is_operator(t));
    return ops[t->type].numofoperands;
    return 0;
}

void
pcalc_perform_operation_from_queue( Token *t,
                                    struct ast_computation_stack *stack )
{
    bool result = 0;
    bool v1 = 0;
    bool v2 = 0;
    uint numofoperands = operator_numofoperands(t);
    if (! ((stack->num_bits) >= (operator_numofoperands(t)) )) {
        goto not_enough_operands;                                 
    }

    bool v[3]; 
    assert_msg( numofoperands < ARRAY_LEN(v),
                "STATIC ASSERT: Language will support at most 3 operands for operators");
    
    for ( uint i = 0; i < numofoperands; i ++ ) {
        v[i] = ast_computation_stack_pop_value(stack);
    }

    // NOTE v[0] Contains the last operand v[numberofoperands-1] contains the first operand

    switch (t->type) {
    case TT_PUNCT_BOTHDIR_ARROW: { result = (v[0] && v[1]) || !((v[0] || v[1])); } break;
    case TT_PUNCT_ARROW: { result = (v[0] == true && v[1] == false ) ? false : true; } break;

    case TT_PUNCT_LOGICAL_AND:
    case TT_PUNCT_BITWISE_AND: { result = v[0] && v[1]; } break;

    case TT_PUNCT_LOGICAL_OR:
    case TT_PUNCT_BITWISE_OR: { result = v[0] || v[1]; } break;

    case TT_PUNCT_LOGICAL_NOT:
    case TT_PUNCT_BITWISE_NOT: { result = !v[0]; } break;

    case TT_PUNCT_EQUAL:
    case TT_PUNCT_EQUAL_EQUAL: { result = (v[0] == v[1]); } break;

    case TT_PUNCT_COMMA: { result = (v[0]); } break; /* // or (v[0], v[1]) c-alike comma */

    default: {
        assert_msg(0, "Invalid code path we should assert before when we require "
            "the number of operands in operator_numofoperands");
    } break;
    }
    
    ast_computation_stack_push(stack, result);
    return;
not_enough_operands: {
        assert_msg(0, "Operator needs more operand, not found enough inside the stack");
        return;
    }
}

bool
token_constant_to_bool( Token *t,
                        bool *out )
{
    bool result = false;
    assert(t->type == TT_CONSTANT);
    char temp = t->text[t->text_len];
    t->text[t->text_len] = 0; {
        char *endptr = NULL;
        unsigned long int lli =  strtoul (t->text, &endptr, 10);

        if ( endptr == NULL || *endptr == '\0') {
            // Valid Character
            result = true;
            *out = (lli == 0) ? 0 : 1;            
        } else {
            result = false;
            *out = 0;
        }
    }
    t->text[t->text_len] = temp;
    return result;
}


static inline void
pcalc_print_tabular(void)
{
    printf("\t");
}


void
pcalc_encoded_compute_with_value( struct ast_token_queue *queue,
                                  struct symbol_table *symtable,
                                  struct ast_truth_table_packed *ast_ttp )
{
    assert(ast_ttp->num_bits > 0);

    struct ast_computation_stack stack;
    stack.num_bits = 0;

    // In the future this if will check for valid allocation
    if ( stack.bits ) {
        Token *t;
        size_t it;
    
        ast_token_queue_for(it, *queue, t) {
            if ( t->type == TT_IDENTIFIER  || t->type == TT_CONSTANT) {
                bool value;
                if ( t->type == TT_IDENTIFIER ) {
                    size_t bit_index = symbol_table_get_identifier_value(symtable, t);

                    value = ast_truth_table_unpack_bool( ast_ttp,
                                                              bit_index);
                    // printf(" bit_index: %zx | unpacked bool: %d\n", bit_index, value);
                } else if (t->type == TT_CONSTANT ) {
                    bool s = token_constant_to_bool( t, & value );
                    assert_msg( s == true, "Passed constant was not a valid boolean, or it was way too big");
                }

                ast_computation_stack_push( & stack, value );
                
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack
                pcalc_perform_operation_from_queue( t, & stack ) ;
                printf("%d", ast_computation_stack_peek_value(& stack));
                pcalc_print_tabular();
            }
            
        }
        assert_msg(stack.num_bits == 1, "Stack should remain with 1 value only, malformed formula");
        //printf("### Final Result: %d\n", ast_computation_stack_pop_value(& stack));
    }
}

void
build_symbol_table_from_queue ( struct ast_token_queue *queue,
                                      struct symbol_table *symtable )
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
printf_token_text(Token *token)
{
    printf("%.*s", token->text_len, token->text);
}




// returns the index of the last read elem
void
pcalc_printf_subformula_recursive(struct ast_token_queue *queue,
                                  size_t index)
{
    Token *t = &(queue->tokens[index]);
    if ( t->type == TT_IDENTIFIER || t->type == TT_CONSTANT ) {
        printf_token_text(t);
    } else if (token_is_operator(t)) {
        assert(token_is_operator(t));
        
        uint numofoperands = operator_numofoperands(t);
        assert_msg(index >= numofoperands, "Inconsistent formula");

        printf(index == (queue->num_tokens - 1) ? "result: (" : "(");
        printf_token_text(t);

        for( size_t it = 1; it <= numofoperands; it++ ) {
            printf(" ");

            // Fixes order of the operands on the queue, A & B, in the queue
            // becomes { [0] = A, [2] = B, [3] = &} Which means that to know
            // the position of the first operand i need to process recursively
            // the second operand. This do { } while fixes this problem but
            // introduces more iteration loops. To make it more efficient
            // SInce we probably will always have operators up to 3 operands
            // while looping for the first operand we can store the postion
            // of the first 2 to cut down on the number of iterations
            size_t newindex = index; {
                if ( index ) { newindex = index - 1; }
                uint operand_num = numofoperands;
                do {
                    /* printf("| newindex: %zu | operand_num: %zu\n", */
                    /*        newindex, operand_num); */
                    Token *t = &(queue->tokens[newindex]);
                    if ( operand_num == (it) ) {
                        break;
                    }
                    if (token_is_operator(t)) {
                        operand_num += operator_numofoperands(t);
                    } else {
                        operand_num--;
                    }
                } while( newindex != 0 ? newindex-- : newindex);
            };
            pcalc_printf_subformula_recursive(queue, newindex);
        }
        printf(")");
    } else {
        invalid_code_path("");
    }
}


void
pcalc_printf_computation_header(struct symbol_table *symtable,
                                struct ast_token_queue *queue)
{
    int s_it;
    char *key;
    void *value; (void) value;
    ast_symbol_table_for(s_it, symtable, key, value) {
        printf("%s", key);
        pcalc_print_tabular();
    }
    Token *t;
    size_t q_it;

    ast_token_queue_for(q_it, *queue, t) {
        if ( token_is_operator(t) ) {
            pcalc_printf_subformula_recursive(queue, q_it);
            pcalc_print_tabular();
        }
    }
        
    
    printf("\n");
    printf("\n");
}

void
pcalc_printf_variables_combination( struct symbol_table *symtable,
                                    struct ast_truth_table_packed *ast_ttp )
{
    int s_it;
    char *key;
    void *value; (void) value;

    int it = 0;
    char *k;
    void *v;
    ast_symbol_table_for(it, symtable, k, v) {
        size_t index = (size_t) v;
        bool bool_value = ast_truth_table_unpack_bool(ast_ttp, index);
        printf("%d", bool_value);
        pcalc_print_tabular();
    }
}

void
bruteforce_solve(struct ast_token_queue *queue)
{
    struct symbol_table symtable = symbol_table_new();


    build_symbol_table_from_queue(queue, &symtable);
    symbol_table_preprocess_ids(& symtable);
    
    struct ast_truth_table_packed ast_ttp;
    ast_truth_table_packed_init_from_symtable(&  ast_ttp, & symtable);

    if ( ast_ttp.bits && ast_ttp.num_bits ) {

        pcalc_printf_computation_header(& symtable, queue);
        
        size_t max_it = 1 << symbol_table_num_ids(& symtable);


        for (size_t i = 0; i < max_it; i ++ ) {
#       if 0
            ast_truth_table_packed_dbglog(& ast_ttp);
#       endif
            // Use the value right here and compute
            pcalc_printf_variables_combination( &symtable, & ast_ttp );
            {
                pcalc_encoded_compute_with_value(queue, & symtable, & ast_ttp );
                printf("\n");
            }

            ast_truth_table_packed_generate_next_combination(& ast_ttp );
        }
    }
}


#define TEST_C_IMPL
#include "test.c"



void
pcalc_process( char *code, size_t codesize)
{
    assert(    code[codesize - 1] == '\0'
            && code[codesize - 2] == '\0'
            && code[codesize - 3] == '\0'
            && code[codesize - 4] == '\0' );

    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, code,
                                codesize,
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

        // log_token(& token);

        // Shunting-yard algorithm
        // @NOTE: Does not handle functions
        {
            if (token.type == TT_CONSTANT ||
                token.type == TT_IDENTIFIER ) {
                ast_token_queue_push(&queue, &token);
            } /* else if ( is function ) */
            else if ( token_is_operator(& token )) {
                Token *peek = NULL;
                while ( ( (stack.num_tokens) != 0 && (peek = ast_token_stack_peek_addr(&stack)))
                        && ( ((op_greater_precedence(peek, & token))
                              || (op_eq_precedence(peek, &token) && op_is_left_associative(peek)))
                             && (peek->type != TT_PUNCT_OPEN_PAREN))) {
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

#if 0
    ast_token_queue_for(it, queue, t) {
        log_token(t);
    }
    printf("\n\n");
#endif

    bruteforce_solve(& queue);

}


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
    char huge_code [] =
        "((!A && B ) || C && (G <-> D) <-> F) || "
        "a1 | a2 | a3 | a4 | a5 | a6 | a7 | a8 | a9 | a10 | a11 |"
        "a12 | a13 | a14 | a15 | a16 | a17 | a18 | a19 | a20 | a21 | a22 |"
        "a23 | a24 | a25 | a26 | a27 | a28 | a29 | a30 | a31 | a32 | a33 |"
        "a34 | a35 | a36 | a37 | a38 | a39 | a40 | a41 | a42 | a43 | a44 |"
        "a45 | a46 | a47 | a48 | a49 | a50 | a51 | a52 | a53 | a54 | a55 |"
        "a56 | a57 | a58 | a59 | a60 | a61 | a62 | a63 | a64 | a65 | a66 |"
        "a67 | a68 | a69 | a70 | a | b | c | d | e | f | g |"
        "h | i | j | k | l | m | n | o | p | q | r"
        "\0\0\0\0\0\0";


    char small_code[] =
        "((!A && B ) || C && (G <-> D) <-> F)"
        "\0\0\0\0\0\0";


    char input_line[4096];
    size_t input_line_len = 0;
    enum { EXTRA_SPACE_FOR_NULL_TERMINATION = 5};
    char *readres = NULL;

    	
    char *formula = NULL;
    size_t formula_size;


    
#if 0
    pcalc_process(small_code, sizeof(small_code));
#else
    while ( 1 ) {
        if ( formula ) { free(formula); formula_size = 0; }
        printf("\n\n");
        user_interact(& formula, & formula_size);
        if ( formula ) {
            printf("\n\n\n");
            pcalc_process(formula, formula_size);
        }
    }
#endif
}





#endif /* MAIN_C_IMPL */

