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
eval_operator( struct ast_node *node,
               struct vm_stack *vms )
{

    assert(node->type == AST_NODE_TYPE_OPERATOR);
    bool result = 0;
    uint numofoperands = operator_num_operands(node);
    if (! ((vms->num_bits) >= (operator_num_operands(node)) )) {
        goto not_enough_operands;                                 
    }

    bool v[3];
    assert_msg( numofoperands <= ARRAY_LEN(v),
                "STATIC ASSERT: Language will support at most 3 operands for operators");
    
    for ( uint i = 0; i < numofoperands; i ++ ) {
        v[i] = vm_stack_pop_value(vms);
    }

    // NOTE v[0] Contains the last operand v[numberofoperands-1] contains the first operand

    switch (node->op) {
            case OPERATOR_IMPLY: {
        // or equivalently
        // result = (!v[1]) || v[0];
        result = (v[1] == true && v[0] == false) ? false : true;
        vm_stack_push(vms, result);
    } break;

        
    case OPERATOR_AND: {
        result = v[0] && v[1];
        vm_stack_push(vms, result);
    } break;

    case OPERATOR_OR: {
        result = v[0] || v[1];
        vm_stack_push(vms, result);
    } break;

    case OPERATOR_XOR: {
        result = v[0] ^ v[1];
        vm_stack_push(vms, result);
    } break;
        
    case OPERATOR_NEGATE: {
        result = !v[0];
        vm_stack_push(vms, result);
    } break;

    case OPERATOR_EQUAL: {
        result = (v[0] == v[1]);
        vm_stack_push(vms, result);
    } break;

    case OPERATOR_TERNARY: {
        result = (v[2] ? v[1] : v[0]);
        vm_stack_push(vms, result);
    } break;

#if 0
    case TT_PUNCT_SEMICOLON:
    case TT_PUNCT_QUESTION_MARK: {
        // There's nothing todo here (it pops and push the same value again)
        // The question mark has the sole purpose of a marker to confine to
        // the right number of operands for the other operators
        vm_stack_push(vms, v[0]);
    } break;
#endif
    case OPERATOR_NOT_EQUAL: {
        result = (v[0] != v[1]);
        vm_stack_push(vms, result);
    } break;

    case OPERATOR_GREATER: {
        result = (v[1] > v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case OPERATOR_LESS: {
        result = (v[1] < v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case OPERATOR_GREATER_EQUAL: {
        result = (v[1] >= v[0]);
        vm_stack_push(vms, result);
    } break;
        
    case OPERATOR_LESS_EQUAL: {
        result = (v[1] <= v[0]);
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
valid_constant ( struct ast_node *node )
{
    bool result = false;
    assert(node->type == AST_NODE_TYPE_CONSTANT);
    if (node->text_len == 1) {
        if ( *(node->text) == '0') {
            result = true;
        } else if ( *(node->text) == '1') {
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
ast_node_constant_to_bool( struct ast_node *node )
{
    assert(valid_constant(node));
    bool result = false;
    assert(node->type == AST_NODE_TYPE_CONSTANT);
    if (node->text_len == 1) {
        if ( *(node->text) == '0') {
            result = false;
        } else if ( *(node->text) == '1') {
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
        struct ast_node *node;
        size_t it;
    
        ast_for(it, *ast, node) {
            if ( node->type == AST_NODE_TYPE_IDENTIFIER  || node->type == AST_NODE_TYPE_CONSTANT) {
                bool value;
                if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                    struct symbol_info *syminfo = symtable_get_syminfo(symtable, node->text, node->text_len);
                    assert_msg(syminfo, "This may fail ? ? ?\n Technically no, if there's the preprocess phase");
                    size_t vmi_index = syminfo->vmi_index;

                    value = vm_inputs_get_value( vmi, vmi_index );
                    // printf(" bit_index: %zx | unpacked bool: %d\n", bit_index, value);
                } else if ( node->type == AST_NODE_TYPE_CONSTANT ) {
                    value = ast_node_constant_to_bool(node);
                }

                vm_stack_push( vms, value );
            } else {
                // Token is an operator: Needs to perform the operation
                //                       and push it into the stack
                eval_operator( node, vms ) ;
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
    struct ast_node *node;
    size_t it;
    ast_for(it, *ast, node) {
        if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
            if (! symtable_is_sym( symtable, node->text, node->text_len )) {
                symtable_add_identifier(symtable, node->text, node->text_len);
            }
        }
    }
}


static inline void
intpt_print_node(struct interpreter *intpt,
                struct ast_node *node)
{
    FILE *f = intpt->stream_info ? intpt->stream_info : stdout;
    ast_node_print(f, node);
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
    assert(operator_index < ast->num_nodes);
    
    assert( ast_node_is_operator(& ast->nodes[operator_index]));
    uint it = 1;
    do {
        assert(it > 0);
        struct ast_node *node = &(ast->nodes[fixed_index]);                
        if ( operand_num == (it) &&
             fixed_index != operator_index) {
            break;
        }

        if (ast_node_is_operator(node)) {
            it += operator_num_operands(node);
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
    struct ast_node *node = & ( ast->nodes[index] );
    if ( node->type == AST_NODE_TYPE_IDENTIFIER || node->type == AST_NODE_TYPE_CONSTANT ) {
        intpt_print_node(intpt, node);
    } else if (ast_node_is_operator(node)) {
        assert(ast_node_is_operator(node));
        
        uint numofoperands = operator_num_operands(node);
        assert_msg(index >= numofoperands, "Inconsistent formula");

        intpt_out_printf(intpt, index == (ast->num_nodes - 1) ? "result: (" : "(");
        intpt_print_node(intpt, node);
        

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

    struct ast_node *node;
    size_t it;
    ast_for(it, *ast, node) {
        if ( ast_node_is_operator(node) ) {
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
        struct symbol_info *syminfo = (struct symbol_info*) v;
        assert(syminfo);
        size_t vmi_index = syminfo->vmi_index;
        bool bool_value = vm_inputs_get_value(vmi, vmi_index);
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

    // Assigns a unique identifier index inside the vm_inputs
    ast_symtable_for(it1, symtable, id_name, value) {
        struct symbol_info *syminfo = (struct symbol_info*) value;
        syminfo->vmi_index = (it2 ++);
    }
}




void
ast_representation_dbglog(struct interpreter *intpt)
{
    struct ast *ast = & intpt->ast;
    struct ast_node *node;
    size_t it;

    intpt_info_printf(intpt, "AST DEBUG LOG: ################################\n");
    // Debug expression printing
    ast_for(it, *ast, node) {
        if ( ast_node_is_operator(node) ) {
            ast_print_expr(intpt, it);
            print_tab(intpt);
        }
    }
    intpt_info_printf(intpt, "\n###############################################\n");
    intpt_info_printf(intpt, "\n\n");
}





bool
dpll_recurse(struct interpreter *intpt,
             struct ast_node *head)
{



    return false;
}



bool
dpll_is_empty_clause( struct interpreter *intpt,
                            struct ast *clauses_ast )
{
    bool result = true;
    size_t it = 0;
    struct ast_node *node = NULL;
    ast_for(it, *clauses_ast, node) {
        if ( node->type == AST_NODE_TYPE_IDENTIFIER
             || node->type == AST_NODE_TYPE_CONSTANT) {
            result = false;
            break;
        }
    }
    assert ( result == true && clauses_ast->num_nodes == 0);
    
    return result;
}

bool
dpll_is_consistent( struct interpreter *intpt,
                    struct ast *clauses_ast )
{
    bool result = true;
    struct ast_node *node = NULL;


    size_t it = 0;
    ast_for(it, *clauses_ast, node) {
        if ( ast_node_is_operator(node) ) {
            if (node->op != OPERATOR_AND) {
                result = false;
                break;
            }
        } else if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
            // There's still an unassigned value
            result = false;
            break;
        } else if ( node->type == AST_NODE_TYPE_CONSTANT ) {
            if (ast_node_constant_to_bool( node ) == false) {
                result = false;
                break;
            }
        } else if ( node->type == AST_NODE_TYPE_DELIMITER ) {
            
        } else {
            result = false;
            break;
        }
    }

    // If gone trough the entire iteration, the result is true
    return result;
}

struct ast_node *
dpll_next_unit_clause(struct interpreter *intpt,
                      struct ast *clauses_ast,
                      struct ast_node **node)
{
    assert(node);
    struct ast_node *result = NULL;
    struct ast_node *start;
    
    if (clauses_ast->num_nodes == 0) {
        *node = NULL;
        return NULL;
    }
    
    if ( *node ) {
        // Find next from this one
        start = (*node) - 1;
    } else {
        // Start from the head
        start = & clauses_ast->nodes[clauses_ast->num_nodes - 1];
    }
    if ( !((start >= clauses_ast->nodes)
           && (start < & clauses_ast->nodes[clauses_ast->num_nodes]))) {
        // Not in bounds
        *node = NULL;
        return NULL;
    } else {
        struct ast_node *n = start;
        for ( n = start; n >= clauses_ast->nodes; n-- ) {
            if (n->type == AST_NODE_TYPE_OPERATOR) {
                if ( n->op != OPERATOR_AND ) {
                    // skip childs
                    size_t operator_index = n - clauses_ast->nodes;
                    size_t first_operand_index = ast_get_operand_index( clauses_ast, operator_index, 1);
                    n = (& clauses_ast->nodes[first_operand_index]) - 1;
                }
            } else if ( n->type == AST_NODE_TYPE_IDENTIFIER ||
                        n->type == AST_NODE_TYPE_CONSTANT) {
                // return this
                result = n;
                break;
            } else {
                assert_msg(0, "Invalid code path for now");
            }
        }
    }

    *node = result;
    return result;
    
}

// input in the ast there should be a formula
// of this kind: c1 & c2 & c3 & c4
// Where ci denotes the i-th unit clause (a or ~a) and `&` the `AND` operator
bool
dpll_solve(struct interpreter *intpt,
           struct ast *clauses_ast)
{
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    intpt_info_printf(intpt, "Input to the dpll solver is:\n\t");
    ast_representation_dbglog(intpt);

/* Algorithm DPLL */
/*   Input: A set of clauses Φ. */
/*   Output: A Truth Value. */

/* function DPLL(Φ) */

//  I still do not understand this one
/*    if Φ is a consistent set of literals */
/*        then return true; */
    if ( dpll_is_consistent(intpt, clauses_ast) )
        return true;
    
//    AST Has been reduced to 0 nodes
/*    if Φ contains an empty clause */
/*        then return false; */
    if ( dpll_is_empty_clause(intpt, clauses_ast))
        return false;

        
// Literal atomic formula or its negation: any symbol and constants
// a unit clause: clauses that are composed of a single literal.
// Because each clause needs to be satisfied, we know that this literal must be true
/*    for every unit clause {l} in Φ */
/*       Φ ← unit-propagate(l, Φ); */
    struct ast_node *node = NULL;
    while ( dpll_next_unit_clause(intpt, clauses_ast, &node) ) {

    }
        


// A Pure literal is any literal that does not appear with its' negation in the formula
//        a & b  // a and b are both pure
//        a & (!a | b)   // a is not pure in this case
/*    for every literal l that occurs pure in Φ */
/*       Φ ← pure-literal-assign(l, Φ); */

/*    l ← choose-literal(Φ); */
/*    return DPLL(Φ ∧ {l}) or DPLL(Φ ∧ {not(l)}); */
    
    
#if 0
    int it = 0, it2 = 0;
    char *id_name;
    void *value; (void) value;
    
    ast_symtable_for(it, symtable, id_name, value) {
        struct symbol_info *syminfo = (struct symbol_info*) value;
        size_t vmi_index = syminfo->vmi_index;
        bool has_value_assigned = syminfo->has_value_assigned;
        if ( ! has_value_assigned ) {
            vm_inputs_assign_value(vmi, 1, vmi_index);
            syminfo->has_value_assigned = true;
            syminfo->value = true;

            // propagate
        }
    }
#endif


    return false;
}


void
bruteforce_solve(struct interpreter *intpt)
{
    struct ast *ast = & intpt->ast;
    struct symtable *symtable =  & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    struct vm_stack *vms = & intpt->vms;
    
    intpt_print_header(intpt);
        
    size_t max_it = 1 << symtable_num_syms(symtable);


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



bool
ast_check_validity( struct interpreter *intpt)
{
    return true;
}




void
ast_node_swap( struct interpreter *intpt,
               size_t ast_node1_index,
               size_t ast_node2_index )
{
    assert(intpt);
    assert(ast_node1_index < intpt->ast.num_nodes);
    assert(ast_node2_index < intpt->ast.num_nodes);

    void *dest, *src;
    size_t n = 10;
    
    //memmove(dest, src, n);
}

bool
ast_to_prenex_form( struct interpreter *intpt)
{

    assert_msg(0, "Needs implementation, for now we always suppose to have prenex formula");
    size_t it;
    struct ast *ast = & (intpt->ast);
    struct ast_node *node;
    
    ast_for(it, *ast, node) {
        for(size_t operands = 1; operands <= operator_num_operands(node); operands++) {
            size_t operand_index = ast_get_operand_index( ast, it, operands);

            struct ast_node *child_node = & (ast->nodes[operand_index]);
            if (child_node->type == AST_NODE_TYPE_OPERATOR
                && (child_node->op == OPERATOR_ENUMERATE || child_node->op == OPERATOR_EXIST)) {
                if (node->type == AST_NODE_TYPE_OPERATOR && node->op == OPERATOR_AND) {
                    
                }
            }
        }
            
    }


    return true;
    goto failed; // @ NOTE: Compiler shut up!
    
failed: {
        return false;
    }
}



// I'm too difficult to understand don't even bother.
// This entire function was a super hack/extension
// around shunting yard algorithm. All the time spent to make this
// work, was probably better invested into reading a book about
// how to properly implement a full descent parser
// ---
// If you're feeling adventurous you can rewrite this entire
// function and use a full descent parser instead.
// This function is pretty self-contained. The interpreter
// cares only about the AST which is pretty stable at this point.
bool
ast_build_from_command( struct interpreter *intpt,
                        char *commandline, size_t commandline_len )
{
    struct ast *ast = & intpt->ast;
    ast_clear(ast);

    // Ensure null-termination, It is not strictly necessary
    // i think, but it may require further testing
    assert(commandline[commandline_len] == 0);


#define SHUNTING_YARD_DEBUG 0
    
#if SHUNTING_YARD_DEBUG == 1
#define SHUNT_DBG() do { ast_node_stack_dbglog( & stack ); ast_dbglog( ast ); printf("\n"); } while(0);
#else
#define SHUNT_DBG() do { } while(0)
#endif


    // 1 token look ahead
    Token t[2] = { Empty_Token, Empty_Token };
    Token *prev_t = NULL;
    Token *curr_t = t;
    
    Tokenizer tknzr;
    tokenizer_init_from_memory( &tknzr, commandline,
                                commandline_len,
                                "*commandline*");

    // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

    // operator stack
    static struct ast_node_stack stack;
    stack.num_nodes = 0;


    size_t va_args_noperands = 0;
    
    while (get_next_token( &tknzr, curr_t)) {
        SHUNT_DBG();
        struct ast_node node;
        // NOTE: At every iteration the tokenizer error is cleared with the call to get_next_token
        if ( tknzr.err ) {
            puts(tknzr.err_desc);
            intpt_info_printf( intpt, " ### Internal parsing error\n");
            goto parse_failed;

        }
        bool conversion = ast_node_from_token(&node, curr_t, prev_t);
        if ( !conversion ) {
            intpt_info_printf(intpt, " ### Parsing error\n ### Token `%.*s` is not supported\n", curr_t->text_len, curr_t->text );
            goto parse_failed;
        }

        {
            if ( node.type == AST_NODE_TYPE_CONSTANT
                 || node.type == AST_NODE_TYPE_IDENTIFIER ) {
                node.num_operands = 0;
                ast_node_stack_push( & stack, & node );
            } else {
                if ( node.type == AST_NODE_TYPE_OPERATOR ) {
                    bool ispostfix = is_postfix_operator( & node);
                    struct ast_node *peek = NULL;
                    while ((stack.num_nodes != 0) && (peek = ast_node_stack_peek_addr(&stack))) {
                        if ( !(is_prefix_delimiter(peek) || is_infix_delimiter(peek))
                             && ( (peek->type == AST_NODE_TYPE_IDENTIFIER || peek->type == AST_NODE_TYPE_CONSTANT)
                                  || (op_greater_precedence(peek, & node))
                                  || ((op_eq_precedence(peek, & node)) && (op_is_left_associative(peek))))) {
                            ast_push(ast, peek);
                            ast_node_stack_pop( & stack);
                        } else { break; }
                    }
                    if ( ispostfix ) {
                        ast_push(ast, & node);
                    } else {
                        ast_node_stack_push( & stack, & node);
                    }
                } else {
                    if ( is_prefix_delimiter(& node) || is_infix_delimiter( & node)) {
                        ast_node_stack_push( & stack, & node);
                        if ( is_infix_delimiter( &node )) {
                            va_args_noperands++;
                        }
                    }
                    if (is_postfix_delimiter(& node) || is_infix_delimiter( & node)) {
                        struct ast_node *peek = NULL;

                        while ((stack.num_nodes != 0) && (peek = ast_node_stack_peek_addr(&stack))) {
                            if ( ! (is_prefix_delimiter(peek))) {

                                if ( !is_infix_delimiter(peek)) {
                                    ast_push(ast, peek);
                                }
                                
                                ast_node_stack_pop( & stack);

                            } else { break; }
                        }
                        if ( stack.num_nodes == 0 ) {
                            if ( peek && ! (is_prefix_delimiter(peek) || is_infix_delimiter(peek))) {
                                if (!(node.type == AST_NODE_TYPE_DELIMITER && node.del == POSTFIX_DELIMITER_SEMICOLON)) {
                                    // Mismatched parentheses
                                    printf("Vaffanculo\n");
                                    intpt_info_printf(intpt, " ### Mismatched parens\n");
                                    goto parse_failed;
                                }
                            }
                        } else {
                            // pop the open paren from the stack
                            if ( peek && (!is_infix_delimiter(& node))) {
                                if ( is_prefix_delimiter(peek)) {
                                    ast_node_stack_pop( & stack );
                                    peek->num_operands = va_args_noperands + 1;
                                    if ( (peek->type == AST_NODE_TYPE_OPERATOR)
                                         && (peek->op == OPERATOR_FNCALL ||
                                             peek->op == OPERATOR_INDEX )) {
                                        (peek->num_operands) += 1;
                                    } else if ((peek->type == AST_NODE_TYPE_OPERATOR)
                                               && (peek->op == OPERATOR_COMPOUND)) {
                                        peek->num_operands = 1;
                                    }
                                    if (!(peek->type == AST_NODE_TYPE_DELIMITER && peek->del == PREFIX_DELIMITER_PAREN && (peek->num_operands == 1))) {
                                        ast_push(ast, peek);
                                    }
                                    va_args_noperands = 0;
                                } else if ( is_infix_delimiter(peek)) {
                                    invalid_code_path();
                                    ast_node_stack_pop( & stack );
                                }
                            }
                        }
                    }
                }
            }
        }
        prev_t = curr_t;
        curr_t += 1;
        if (curr_t >= (t + ARRAY_LEN(t))) { curr_t = t; }
    }

#if SHUNTING_YARD_DEBUG == 1
    printf("\n\nOut of main loop: \n\n");
#endif

    /* if there are no more tokens to read: */
    /* 	while there are still operator tokens on the stack: */
    /* 		/\* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. *\/ */
    /* 		pop the operator from the operator stack onto the output queue. */

    struct ast_node *peek = NULL;
    while ( ( (stack.num_nodes) != 0 && (peek = ast_node_stack_peek_addr(&stack)))) {
            
        SHUNT_DBG();
        if ( is_prefix_delimiter(peek) || is_postfix_delimiter(peek)) {
            intpt_info_printf( intpt, " ### Mismatched parens\n");
            goto parse_failed;
        }
                                
        if ( !is_infix_delimiter(peek)) {
            ast_push(ast, peek);
        }
        ast_node_stack_pop( & stack);
    }

#if SHUNTING_YARD_DEBUG == 1
    printf("Final: \n");
    SHUNT_DBG();
#endif
    
    ast_node_stack_dbglog( & stack );
    ast_dbglog( ast );
    printf("\n");

    return true;
    
parse_failed: {
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
    intpt->ast.num_nodes = 0;
    intpt->vmi.num_inputs = 0;

    

    struct ast *ast = & intpt->ast;
    assert(intpt->vms.num_bits == 0);
    assert(intpt->ast.num_nodes == 0);
    assert(symtable_num_syms(& (intpt->symtable)) == 0);
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

    struct ast_node *node;
    size_t it;
    ast_for(it, *ast, node) {
        if ( node->type == AST_NODE_TYPE_CONSTANT ) {
            bool valid = valid_constant(node);
            if ( ! valid ) {
                intpt_info_printf(intpt, " ### %.*s is not a valid constant: valid constants are {`0`, `1`}\n", node->text_len, node->text);
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
        struct ast_node *node = NULL;
        while ( dpll_next_unit_clause(intpt, ast, &node) ) {
            int lezzo = 1;
            ast_node_print(stdout, node);
        }
        int prova = 1;
        // bruteforce_solve(intpt);
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
