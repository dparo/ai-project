#ifndef INTERPRETER_C_INCLUDE
#define INTERPRETER_C_INCLUDE

#define INTERPRETER_UTILS_C_IMPL
#include "interpreter-utils.c"

struct interpreter {
    struct vm_stack vms;
    struct ast ast;
    struct symtable symtable;
    struct vm_inputs vmi;
};


//#######################################################
#endif /* INTERPRETER_C_INCLUDE */
#if !defined INTERPRETER_C_IMPLEMENTED && defined INTERPRETER_C_IMPL
#define INTERPRETER_C_IMPLEMENTED
//#######################################################




void
eval_operator( Token *t,
               struct vm_stack *vms )
{
    bool result = 0;
    bool v1 = 0;
    bool v2 = 0;
    uint numofoperands = operator_numofoperands(t);
    if (! ((vms->num_bits) >= (operator_numofoperands(t)) )) {
        goto not_enough_operands;                                 
    }

    bool v[3]; 
    assert_msg( numofoperands < ARRAY_LEN(v),
                "STATIC ASSERT: Language will support at most 3 operands for operators");
    
    for ( uint i = 0; i < numofoperands; i ++ ) {
        v[i] = vm_stack_pop_value(vms);
    }

    // NOTE v[0] Contains the last operand v[numberofoperands-1] contains the first operand

    switch (t->type) {
    case TT_PUNCT_BOTHDIR_ARROW: { result = (v[0] && v[1]) || !((v[0] || v[1])); } break;
    case TT_PUNCT_ARROW: { result = (v[1] == true && v[0] == false ) ? false : true; } break;

    case TT_PUNCT_LOGICAL_AND:
    case TT_PUNCT_BITWISE_AND: { result = v[0] && v[1]; } break;

    case TT_PUNCT_LOGICAL_OR:
    case TT_PUNCT_BITWISE_OR: { result = v[0] || v[1]; } break;

    case TT_PUNCT_BITWISE_XOR: { result = v[0] ^ v[1]; } break;
        
    case TT_PUNCT_LOGICAL_NOT:
    case TT_PUNCT_BITWISE_NOT: { result = !v[0]; } break;

    case TT_PUNCT_EQUAL:
    case TT_PUNCT_EQUAL_EQUAL: { result = (v[0] == v[1]); } break;

    case TT_PUNCT_NOT_EQUAL : { result = ( v[0] != v[1]); } break;
        
    case TT_PUNCT_COMMA: { result = (v[0]); } break; /* // or (v[1], v[0]) c-alike comma */
    case TT_PUNCT_SEMICOLON: { result = 0; } break;
        
    default: {
        assert_msg(0, "Invalid code path we should assert before when we require "
            "the number of operands in operator_numofoperands");
    } break;
    }
    
    vm_stack_push(vms, result);
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
print_tab(void)
{
    printf("\t");
}


void
eval_expr( struct interpreter *intpt )
{
    struct ast *ast = &intpt->ast;
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;

    assert(vmi->num_inputs > 0);

    struct vm_stack *vms = & intpt->vms;

    // In the future this if will check for valid allocation
    if ( vms->bits ) {
        Token *t;
        size_t it;
    
        ast_for(it, *ast, t) {
            if ( t->type == TT_IDENTIFIER  || t->type == TT_CONSTANT) {
                bool value;
                if ( t->type == TT_IDENTIFIER ) {
                    size_t input_index = symtable_get_identifier_value(symtable, t);

                    value = vm_inputs_unpack_bool( vmi,
                                                   input_index );
                    // printf(" bit_index: %zx | unpacked bool: %d\n", bit_index, value);
                } else if ( t->type == TT_CONSTANT ) {
                    bool s = token_constant_to_bool( t, & value );
                    assert_msg( s == true, "Passed constant was not a valid boolean, or it was way too big");
                }

                vm_stack_push( vms, value );
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack
                eval_operator( t, vms ) ;
                printf("%d", vm_stack_peek_value(vms));
                print_tab();
            }            
        }
        assert_msg(vms->num_bits == 1, "Stack should remain with 1 value only, malformed formula");
        //printf("### Final Result: %d\n", vm_stack_pop_value(& stack));
    }
}

void
symtable_build_from_queue ( struct interpreter *intpt )
{
    struct ast *ast = &intpt->ast;
    struct symtable *symtable = & intpt->symtable;
    Token *t;
    size_t it;
    ast_for(it, *ast, t) {
        if ( t->type == TT_IDENTIFIER ) {
            //null terminate;
            symtable_add_identifier(symtable, t);
        }
    }
}


static inline void
ast_print_token(Token *token)
{
    printf("%.*s", token->text_len, token->text);
}



// returns the index of the last read elem
void
ast_print_expr ( struct ast *ast,
                 size_t index )
{
    
    Token *t = &(ast->tokens[index]);
    if ( t->type == TT_IDENTIFIER || t->type == TT_CONSTANT ) {
        ast_print_token(t);
    } else if (token_is_operator(t)) {
        assert(token_is_operator(t));
        
        uint numofoperands = operator_numofoperands(t);
        assert_msg(index >= numofoperands, "Inconsistent formula");

        printf(index == (ast->num_tokens - 1) ? "result: (" : "(");
        ast_print_token(t);

        for( size_t it = 1; it <= numofoperands; it++ ) {
            printf(" ");

            // Fixes order of the operands on the queue, A & B, in the queue
            // becomes { [0] = A, [2] = B, [3] = &} Which means that to know
            // the position of the first operand i need to process recursively
            // the second operand. This do { } while fixes this problem but
            // introduces more iteration loops. To make it more efficient
            // Since we probably will always have operators up to 3 operands
            // while looping for the first operand we can store the postion
            // of the first 2 to cut down on the number of iterations
            size_t newindex = index; {
                if ( index ) { newindex = index - 1; }
                uint operand_num = numofoperands;
                do {
                    /* printf("| newindex: %zu | operand_num: %zu\n", */
                    /*        newindex, operand_num); */
                    Token *t = &(ast->tokens[newindex]);
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
            ast_print_expr(ast, newindex);
        }
        printf(")");
    } else {
        invalid_code_path("");
    }
}


void
intpt_print_header( struct interpreter *intpt)
{
    struct symtable *symtable = & intpt->symtable;
    struct ast *ast = &intpt->ast;
    
    int s_it;
    char *key;
    void *value; (void) value;
    ast_symtable_for(s_it, symtable, key, value) {
        printf("%s", key);
        print_tab();
    }
    
    Token *t;
    size_t it;
    ast_for(it, *ast, t) {
        if ( token_is_operator(t) ) {
            ast_print_expr(ast, it);
            print_tab();
        }
    }
        
    
    printf("\n");
    printf("\n");
}


void
intpt_print_inputs( struct interpreter *intpt )
{
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    
    int s_it;
    char *key;
    void *value; (void) value;

    int it = 0;
    char *k;
    void *v;
    ast_symtable_for(it, symtable, k, v) {
        size_t index = (size_t) v;
        bool bool_value = vm_inputs_unpack_bool(vmi, index);
        printf("%d", bool_value);
        print_tab();
    }
}


void
symtable_preprocess_expr ( struct symtable *symtable )
{

    int it1 = 0, it2 = 0;

    char *id_name;
    void *value; (void) value;
    
    ast_symtable_for(it1, symtable, id_name, value) {
        symtable_set_identifier_value(symtable, id_name, it2 ++);
        //printf("k : %s | v: %zu\n", k, (size_t) v);
    }
}


void
bruteforce_solve(struct interpreter *intpt)
{
    struct ast *ast = & intpt->ast;
    struct symtable *symtable =  & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    
    intpt_print_header(intpt);
        
    size_t max_it = 1 << symtable_num_ids(symtable);


    for (size_t i = 0; i < max_it; i ++ ) {
#       if 0
        vm_inputs_dbglog(& vmi);
#       endif
        // Use the value right here and compute
        intpt_print_inputs(intpt);
        {
            eval_expr(intpt);
            printf("\n");
        }

        vm_inputs_generate_next_combination(vmi );
    }
}


#define TEST_C_IMPL
#include "test.c"


void
ast_dbglog(struct ast *ast)
{
    Token *t;
    size_t it;

    ast_for(it, *ast, t) {
        log_token(t);
    }
    printf("\n\n");
    ast_for(it, *ast, t) {
        if ( token_is_operator(t) ) {
            ast_print_expr(ast, it);
            print_tab();
        }
    }
}





void
build_ast_from_user_input( struct ast *ast,
                           char *commandline, size_t commandline_size )
{
    ast_clear(ast);
    assert(    commandline[commandline_size - 1] == '\0'
               && commandline[commandline_size - 2] == '\0'
               && commandline[commandline_size - 3] == '\0'
               && commandline[commandline_size - 4] == '\0' );

    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, commandline,
                                commandline_size,
                                "*commandline*");

    // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

    // operator stack
    static struct token_stack stack;
    stack.num_tokens = 0;
      
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

        // Extensions ->
        //   Postfix operators do an uncoditional push onto
        //                 ast_token_queue_push(queue, &token)
        // 
        // Prefix operators do an uncoditional push onto    ???(Needs testing)???
        //                   ast_token_stack_push(&stack, &token);
        
        // Shunting-yard algorithm
        // @NOTE: Does not handle functions
        {
            if (token.type == TT_CONSTANT ||
                token.type == TT_IDENTIFIER ) {
                ast_push(ast, &token);
            } /* else if ( is function ) */
            else if ( token_is_operator(& token)) {
                if ( is_prefix_operator(& token)) {
                    token_stack_push(&stack, &token);
                } else if (is_postfix_operator(& token)) {
                    ast_push(ast, &token);
                } else {
                    assert(is_infix_operator(&token));
                    Token *peek = NULL;
                    while ( ( (stack.num_tokens) != 0 && (peek = token_stack_peek_addr(&stack))) 
                            && ( (peek->type != TT_PUNCT_OPEN_PAREN &&
                                  ((op_greater_precedence(peek, & token))
                                   || (op_eq_precedence(peek, &token) && op_is_left_associative(peek)))))) {
                        ast_push(ast, peek);
                        token_stack_pop( & stack);
                    }
                    token_stack_push( & stack, & token);
                }
            } else if ( token.type == TT_PUNCT_OPEN_PAREN ) {
                token_stack_push( & stack, & token);
            } else if (token.type == TT_PUNCT_CLOSE_PAREN ) {
                Token *peek = NULL;
                while ( ( (stack.num_tokens) != 0 && (peek = token_stack_peek_addr(&stack)))
                        && ( peek->type != TT_PUNCT_OPEN_PAREN)) {
                    ast_push( ast, peek);
                    token_stack_pop( & stack );
                }
                if ( stack.num_tokens == 0 ) {
                    if ( peek && peek->type != TT_PUNCT_OPEN_PAREN ) {
                        // Mismatched parentheses
                        fprintf(stderr, "Mismatched parens\n");
                        goto parse_end;
                    }
                } else {
                    // pop the closed paren from the stack
                    token_stack_pop( & stack );
                }
            }
        }
        

    }

    /* if there are no more tokens to read: */
    /* 	while there are still operator tokens on the stack: */
    /* 		/\* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. *\/ */
    /* 		pop the operator from the operator stack onto the output queue. */

    Token *peek = NULL;
    while ( ( (stack.num_tokens) != 0 && (peek = token_stack_peek_addr(&stack)))) {
        if ( peek->type == TT_PUNCT_OPEN_PAREN ||
             peek->type == TT_PUNCT_CLOSE_PAREN ) {
            fprintf(stderr, "Mismatched parens\n");
            goto parse_end;
        }
        ast_push( ast, peek);
        token_stack_pop ( & stack );
    }

    
parse_end: {
    }

}


bool
intpt_begin_frame(struct interpreter *intpt )
{
    bool result = true;
    bool symtable_result = false;
    struct symtable *symtable =  & intpt->symtable;
    symtable_clear(symtable);
    if (symtable_is_valid(symtable)) {
        symtable_result = symtable_clear(symtable);
    } else {
        symtable_result = symtable_new(symtable);
    }
    result &= symtable_result;
    
    intpt->vms.num_bits = 0;
    intpt->ast.num_tokens = 0;
    intpt->vmi.num_inputs = 0;

    

    struct ast *ast = & intpt->ast;
    assert(intpt->vms.num_bits == 0);
    assert(intpt->ast.num_tokens == 0);
    assert(symtable_num_ids(& (intpt->symtable)) == 0);
    assert(intpt->vmi.num_inputs == 0);
    return result;
}

void
intpt_end_frame( struct interpreter *intpt)
{
    (void) intpt;
}



bool
ast_preprocess_command ( struct interpreter *intpt )
{
    bool alloc_result = true;
    struct ast *ast = & intpt->ast;
    struct symtable *symtable = & intpt->symtable;
    
    symtable_build_from_queue(intpt);
    symtable_preprocess_expr(symtable);
    
    struct vm_inputs *vmi = & intpt -> vmi;
    vm_inputs_init_from_symtable( vmi, symtable);



/* SUCCESS: */ {
        return 1;
    }

    goto FAILURE; // @NOTE: To remove warning of unused label
FAILURE: {
        fprintf(stderr, "Interpreter: Failed memory allocation\n");
        return 0;
    }
}




bool
eval_ast(struct interpreter *intpt )
{
    bool result = false;
    struct ast *ast = & intpt->ast;
    struct vm_inputs *vmi = & intpt->vmi;

    
    if (ast_preprocess_command (intpt)) {
        if ( vmi->inputs && vmi->num_inputs ) {
            bruteforce_solve(intpt);
        }


        result = true;
    } else {
        result = false;
    }
    return result;
}




void
eval_commandline ( struct interpreter *intpt,
                   char *commandline,
                   size_t commandline_size)
{
    struct ast *ast = & intpt->ast;
    
    if ( intpt_begin_frame(intpt)) {
        build_ast_from_user_input( ast, commandline, commandline_size );
# if 0
        ast_dbglog(ast);
#endif
        if ( eval_ast( intpt ) ) {
            
        }

        intpt_end_frame(intpt);
    } else {
        fprintf(stderr, "Failed to setup Interpreter context for evaluating the command\n");
    }
}



#endif /* INTERPRETER_C_IMPL */
