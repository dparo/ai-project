#ifndef INTERPRETER_C_INCLUDE
#define INTERPRETER_C_INCLUDE

#define INTERPRETER_UTILS_C_IMPL
#include "interpreter-utils.c"

struct interpreter {
    FILE *stream_info;
    FILE *stream_out;
    
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





PRINTF_STYLE(2, 3)
static inline int
intpt_out_printf ( struct interpreter *intpt,
                   char *format,
                   ... )

{
    assert(intpt);
    FILE *f = intpt->stream_out ? intpt->stream_out : stdout;
    va_list ap;
    
    va_start(ap, format);
    int result = vfprintf(f, format, ap);
    va_end(ap);
    return result;
}

PRINTF_STYLE(2, 3)
static inline int
intpt_info_printf ( struct interpreter *intpt,
                    char *format,
                    ... )

{
    assert(intpt);
    FILE *f = intpt->stream_info ? intpt->stream_info : stdout;
    va_list ap;
    
    va_start(ap, format);
    int result = vfprintf(f, format, ap);
    va_end(ap);
    return result;
}


void
eval_operator( Token *t,
               struct vm_stack *vms )
{
    bool result = 0;
    uint numofoperands = operator_numofoperands(t);
    if (! ((vms->num_bits) >= (operator_numofoperands(t)) )) {
        goto not_enough_operands;                                 
    }

    bool v[3];
    assert_msg( numofoperands <= ARRAY_LEN(v),
                "STATIC ASSERT: Language will support at most 3 operands for operators");
    
    for ( uint i = 0; i < numofoperands; i ++ ) {
        v[i] = vm_stack_pop_value(vms);
    }

    // NOTE v[0] Contains the last operand v[numberofoperands-1] contains the first operand

    switch (t->type) {
        
    case TT_PUNCT_ARROW: {
        // or equivalently
        // result = (!v[1]) || v[0];
        result = (v[1] == true && v[0] == false) ? false : true;
        vm_stack_push(vms, result);
    } break;

        
    case TT_PUNCT_COMMA:
    case TT_PUNCT_LOGICAL_AND:
    case TT_PUNCT_BITWISE_AND: {
        result = v[0] && v[1];
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_LOGICAL_OR:
    case TT_PUNCT_BITWISE_OR: {
        result = v[0] || v[1];
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_BITWISE_XOR: {
        result = v[0] ^ v[1];
        vm_stack_push(vms, result);
    } break;
        
    case TT_PUNCT_LOGICAL_NOT:
    case TT_PUNCT_BITWISE_NOT: {
        result = !v[0];
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_EQUAL:
        case TT_PUNCT_BOTHDIR_ARROW:
    case TT_PUNCT_EQUAL_EQUAL: {
        result = (v[0] == v[1]);
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_COLON: {
        result = (v[2] ? v[1] : v[0]);
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_SEMICOLON:
    case TT_PUNCT_QUESTION_MARK: {
        // There's nothing todo here (it pops and push the same value again)
        // The question mark has the sole purpose of a marker to confine to
        // the right number of operands for the other operators
        vm_stack_push(vms, v[0]);
    } break;

    case TT_PUNCT_NOT_EQUAL : {
        result = ( v[0] != v[1]);
        vm_stack_push(vms, result);
    } break;

    case TT_PUNCT_GREATER: {
        result = (v[1] > v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case TT_PUNCT_LESS: {
        result = ( v[1] < v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case TT_PUNCT_GREATER_OR_EQUAL: {
        result = ( v[1] >= v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case TT_PUNCT_LESS_OR_EQUAL: {
        result = ( v[1] <= v[0]);
        vm_stack_push(vms, result);
    } break;

        

    default: {
        assert_msg(0, "Invalid code path we should assert before when we require "
                   "the number of operands in operator_numofoperands");
    } break;
    }

    return;

not_enough_operands: {
        assert_msg(0, "Operator needs more operand, not found enough inside the stack");
        return;
    }
}

bool
valid_constant ( Token *t )
{
    bool result = false;
    assert(t->type == TT_CONSTANT);
    if (t->text_len == 1) {
        if ( *(t->text) == '0') {
            result = true;
        } else if ( *(t->text) == '1') {
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    return result;
}


bool
token_constant_to_bool( Token *t )
{
    assert(valid_constant(t));
    bool result = false;
    assert(t->type == TT_CONSTANT);
    if (t->text_len == 1) {
        if ( *(t->text) == '0') {
            result = false;
        } else if ( *(t->text) == '1') {
            result = true;
        } else {
            // should assert above inside valid_constant(t)
            invalid_code_path();
        }
    } else {
        // should assert above inside valid_constant(t)
        invalid_code_path();
    }
    return result;

}


static inline void
print_tab(struct interpreter *intpt)
{
    intpt_out_printf(intpt, "\t");
}


void
eval_entire_expr( struct interpreter *intpt )
{
    struct ast *ast = &intpt->ast;
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;

# if 0 // This assert is not needed cuz eval_entire_expr may be called with constant like `0 | 1`
    assert(vmi->num_inputs > 0);
#endif


    struct vm_stack *vms = & intpt->vms;
    
    assert(vms->num_bits == 0);
    // assert_msg(0, "Needs to handle it with recursion man");
    
    // In the future this if will check for valid allocation
    // But this check will probably implemented by the preprocessor,
    // so this if could become just an assert.
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
                    value = token_constant_to_bool( t);
                }

                vm_stack_push( vms, value );
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack
                eval_operator( t, vms ) ;
                intpt_out_printf(intpt, "%d", vm_stack_peek_value(vms));
                print_tab(intpt);
            }            
        }
#if 0  // Not really necessary: Example    `a; (B | C)` cannot be avaluated with this assert restriction
        assert_msg(vms->num_bits == 1, "Stack should remain with 1 value only, malformed formula");
#endif
    }
}

void
symtable_build_from_ast ( struct symtable *symtable,
                          struct ast *ast )
{
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
ast_print_token(struct interpreter *intpt,
                Token *token)
{
    FILE *f = intpt->stream_info ? intpt->stream_info : stdout;
    log_token_text(f, token);
}



// Fixes order of the operands on the queue, A & B, in the queue
// becomes { [0] = A, [2] = B, [3] = &} Which means that to know
// the position of the first operand i need to process recursively
// the second operand. This do { } while fixes this problem but
// introduces more iteration loops. To make it more efficient
// Since we probably will always have operators up to 3 operands
// while looping for the first operand we can store the postion
// of the first 2 to cut down on the number of iterations
size_t
ast_get_operand_index( struct ast *ast,
                       size_t operator_index,
                       size_t operand_num)
{
    size_t fixed_index = operator_index;
    assert(operand_num > 0);
    assert(operator_index < ast->num_tokens);
    
    assert( token_is_operator(& ast->tokens[operator_index]));
    uint it = 1;
    do {
        assert(it > 0);
        Token *t = &(ast->tokens[fixed_index]);                
        if ( operand_num == (it) &&
             fixed_index != operator_index) {
            break;
        }

        if (token_is_operator(t)) {
            it += operator_numofoperands(t);
        }
        it--;
    } while( fixed_index != 0 ? fixed_index-- : fixed_index);

    assert(fixed_index != operator_index);
    return fixed_index;
}

void
ast_print_expr ( struct interpreter *intpt,
                 size_t index )
{
    struct ast* ast = & intpt->ast;
    Token *t = & ( ast->tokens[index] );
    if ( t->type == TT_IDENTIFIER || t->type == TT_CONSTANT ) {
        ast_print_token(intpt, t);
    } else if (token_is_operator(t)) {
        assert(token_is_operator(t));
        
        uint numofoperands = operator_numofoperands(t);
        assert_msg(index >= numofoperands, "Inconsistent formula");

        intpt_out_printf(intpt, index == (ast->num_tokens - 1) ? "result: (" : "(");
        ast_print_token(intpt, t);


        for( size_t operand_num = 1;
             operand_num <= numofoperands;
             operand_num++ ) {
            intpt_out_printf(intpt, " ");
            size_t fixed_index = ast_get_operand_index(ast, index, operand_num);
            ast_print_expr(intpt, fixed_index);
        }
        intpt_out_printf(intpt, ")");
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
        intpt_out_printf(intpt, "%s", key);
        print_tab(intpt);
    }

    Token *t;
    size_t it;
    ast_for(it, *ast, t) {
        if ( token_is_operator(t) ) {
            ast_print_expr(intpt, it);
            print_tab(intpt);
        }
    }
        

    intpt_out_printf(intpt, "\n");
    intpt_out_printf(intpt, "\n");
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
        intpt_out_printf(intpt, "%d", bool_value);
        print_tab(intpt);
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
    struct vm_stack *vms = & intpt->vms;
    
    intpt_print_header(intpt);
        
    size_t max_it = 1 << symtable_num_ids(symtable);


    for (size_t i = 0; i < max_it; i ++ ) {
        vm_stack_clear(vms);
#       if 0
        vm_inputs_dbglog(vmi);
#       endif
        // Use the value right here and compute
        intpt_print_inputs(intpt);
        {
            eval_entire_expr(intpt);

            intpt_out_printf(intpt, "\n");
        }

        vm_inputs_increment(vmi);
    }
}


#define TEST_C_IMPL
#include "test.c"


void
ast_representation_dbglog(struct interpreter *intpt)
{
    struct ast *ast = & intpt->ast;
    Token *t;
    size_t it;

    intpt_info_printf(intpt, "AST DEBUG LOG: ################################\n");
    ast_for(it, *ast, t) {
        log_token(t);
    }
    intpt_info_printf(intpt, "\n\n");
    // Debug expression printing
    ast_for(it, *ast, t) {
        if ( token_is_operator(t) ) {
            ast_print_expr(intpt, it);
            print_tab(intpt);
        }
    }
    intpt_info_printf(intpt, "\n########################################\n");
    intpt_info_printf(intpt, "\n\n");
}





bool
ast_build_from_command( struct interpreter *intpt,
                        char *commandline, size_t commandline_len )
{
    struct ast *ast = & intpt->ast;
    ast_clear(ast);

    // Insure null-termination, It is not strictly necessary
    // i think, but it may require further testing
    assert(commandline[commandline_len] == 0);

    
    Token token = Empty_Token;
    bool done = false;
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, commandline,
                                commandline_len,
                                "*commandline*");

    // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

    // operator stack
    static struct token_stack stack;
    stack.num_tokens = 0;


    bool prev_was_identifier = false;
    // NOTE: Maybe better error handling because even the push_state can throw an error
    //       Maybe set a locked variable inside the tokenizer for critical stuff
    //       that will inevitably inject more complexity on the library side
    while (!done && get_next_token( &tknzr, & token)) {
        token_stack_dbglog( & stack );
        ast_dbglog( ast );
        printf("\n");
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
                token_stack_push( & stack, & token);
                // ast_push(ast, &token);
            } /* else if ( is function ) */
            else {
                if ( token.type == TT_PUNCT_OPEN_PAREN ) {
                    token_stack_push( & stack, & token);
                } else if ( token.type == TT_PUNCT_CLOSE_PAREN ) {
                    Token *peek = NULL;
                    while ((stack.num_tokens != 0) && (peek = token_stack_peek_addr(&stack))) {
                        if ( peek->type != TT_PUNCT_OPEN_PAREN ) {
                            ast_push(ast, peek);
                            token_stack_pop( & stack);
                        } else { break; }
                    }
                    if ( stack.num_tokens == 0 ) {
                        if ( peek && peek->type != TT_PUNCT_OPEN_PAREN ) {
                            // Mismatched parentheses
                            intpt_info_printf(intpt, "Mismatched parens\n");
                            goto parse_end;
                        }
                    } else {
                        // pop the closed paren from the stack
                        token_stack_pop( & stack );
                    }
                } else if ( token_is_operator(& token)) {
                    bool ispostfix = is_postfix_operator( &token);
                    Token *peek = NULL;
                    while ((stack.num_tokens != 0) && (peek = token_stack_peek_addr(&stack))) {
                        if ( !(peek->type == TT_PUNCT_OPEN_PAREN)
                             && ( (peek->type == TT_IDENTIFIER || peek->type == TT_CONSTANT)
                                  || (op_greater_precedence(peek, & token))
                                  || ((op_eq_precedence(peek, &token)) && (op_is_left_associative(peek))))) {
                            ast_push(ast, peek);
                            token_stack_pop( & stack);
                        } else { break; }
                    }
                    if ( ispostfix ) {
                        ast_push(ast, & token);
                    } else {
                        token_stack_push( & stack, & token);
                    }
                } else {
                    invalid_code_path();
                }
                prev_was_identifier = false;
            }
        }
    }

    printf("Out of main loop: \n");


    /* if there are no more tokens to read: */
    /* 	while there are still operator tokens on the stack: */
    /* 		/\* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. *\/ */
    /* 		pop the operator from the operator stack onto the output queue. */

    Token *peek = NULL;
    while ( ( (stack.num_tokens) != 0 && (peek = token_stack_peek_addr(&stack)))) {
        token_stack_dbglog( & stack );
        ast_dbglog( ast );
        printf("\n");

        if ( peek->type == TT_PUNCT_OPEN_PAREN ||
             peek->type == TT_PUNCT_CLOSE_PAREN ) {
            intpt_info_printf( intpt, " ### Mismatched parens\n");
            goto parse_end;
        }
        ast_push( ast, peek);
        token_stack_pop ( & stack );
    }

    printf("Final: ");
    token_stack_dbglog( & stack );
    ast_dbglog( ast );
    printf("\n");

    return true;
    
parse_end: {
        intpt_info_printf(intpt, " ### Failed formula parsing\n");
        return false;
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
preprocess_command ( struct interpreter *intpt )
{
    bool alloc_result = true;
    struct ast *ast = & intpt->ast;
    struct symtable *symtable = & intpt->symtable;

    Token *t;
    size_t it;
    ast_for(it, *ast, t) {
        if ( t->type == TT_CONSTANT ) {
            bool valid = valid_constant(t);
            if ( ! valid ) {
                intpt_info_printf(intpt, " ### %.*s is not a valid constant: valid constants are {`0`, `1`}\n", t->text_len, t->text);
                goto INVALID_EXPR;
            }
        }
    }

    symtable_build_from_ast(symtable, ast);
    symtable_preprocess_expr(symtable);
    
    struct vm_inputs *vmi = & intpt -> vmi;
    vm_inputs_init_from_symtable( vmi, symtable);



/* SUCCESS: */ {
        return 1;
    }

INVALID_EXPR: {
        intpt_info_printf(intpt, " ### Semantic or syntactic error\n");
        return 0;
    }
}




bool
eval_ast(struct interpreter *intpt )
{
    bool result = false;
    struct ast *ast = & intpt->ast;
    struct vm_inputs *vmi = & intpt->vmi;

    
    if (preprocess_command (intpt)) {
#if 0
        intpt_print_header(intpt);
#else
        bruteforce_solve(intpt);
#endif


        result = true;
    } else {
        result = false;
    }
    return result;
}




void
eval_commandline ( struct interpreter *intpt,
                   char *commandline,
                   size_t commandline_len )
{
    struct ast *ast = & intpt->ast;
    
    if ( intpt_begin_frame(intpt)) {
        if ( ast_build_from_command( intpt, commandline, commandline_len ) ) {
# if 0
            ast_representation_dbglog(intpt);
# else
            if ( eval_ast( intpt ) ) {
                
            }
#endif
        }
        intpt_end_frame(intpt);
    } else {
        intpt_info_printf(intpt, "Failed to setup Interpreter context for evaluating the command\n");
    }
}



#endif /* INTERPRETER_C_IMPL */
