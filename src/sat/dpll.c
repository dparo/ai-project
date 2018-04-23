#ifndef DPLL_C_INCLUDE
#define DPLL_C_INCLUDE


//#######################################################
#endif /* DPLL_C_INCLUDE */
#if !defined DPLL_C_IMPLEMENTED && defined DPLL_C_IMPL
#define DPLL_C_IMPLEMENTED
//#######################################################


struct ast_node *
dpll_get_expr_subtree_end_node( struct ast *ast,
                                struct ast_node *expr_node)
{
    assert(expr_node >= ast->nodes);
    assert(expr_node < ast_end(ast));

    if ( !ast_node_is_operator(expr_node)) {
        return expr_node;
    } else {
        uint it = 0;
        struct ast_node *node = expr_node;
        for ( ;
              node >= ast_begin(ast);
              node --, it--) {
            if (ast_node_is_operator(node)) {
                it += operator_num_operands(node);
            }
            if ( (it) == 0 ) {
                break;
            }
        }
        assert(node != expr_node);
        return node;
    }

}

void
ast_subtree_push( struct ast_node_stack *out,
                  struct ast *in,
                  struct ast_node *expr_node )
{
    struct ast_node *end_node = dpll_get_expr_subtree_end_node( in, expr_node);

    for (struct ast_node *node = end_node;
         node <= expr_node;
         node ++ ) {
        ast_node_stack_push(out, node);
    }
}

void
dpll_operator_conversion_aux ( struct ast_node *expr_node,
                               struct ast *in,
                               struct ast_node_stack *out )
{
    struct ast_node *node = expr_node;
    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
        struct ast_node *op2_node = ast_get_operand_node (in, node, 2);
        
        if (node->op == OPERATOR_DOUBLE_IMPLY
            || node->op == OPERATOR_EQUAL_EQUAL
            || node->op == OPERATOR_NOT_EQUAL ) {
            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & OR_NODE);
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
            ast_node_stack_push(out, & AND_NODE);
            if (node->op == OPERATOR_NOT_EQUAL )
                ast_node_stack_push(out, & NEGATE_NODE);
                
        } else if (node->op == OPERATOR_IMPLY) {
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
        } else if (node->op == OPERATOR_NOR ) {
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
            ast_node_stack_push(out, & NEGATE_NODE);
        } else if (node->op == OPERATOR_NAND ) {
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & AND_NODE);
            ast_node_stack_push(out, & NEGATE_NODE);
        } else if (node->op == OPERATOR_XOR) {
            // P ^ Q = (~P | ~Q) & (P | Q)
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & OR_NODE);
            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
            ast_node_stack_push(out, & AND_NODE);
        } else if (node->op == OPERATOR_GREATER) {
            // (a > b) ==== (a & ~b)
            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & AND_NODE);           
        } else if (node->op == OPERATOR_GREATER_EQUAL) {
            // (a >= b) ==== (a | ~b)
            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & OR_NODE);
        } else if (node->op == OPERATOR_LESS ) {
            // (a < b) ==== (~a & b)
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & AND_NODE);
        } else if (node->op == OPERATOR_LESS_EQUAL ) {
            // (a <= b) ==== (~a | b)
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
        } else {
            assert_msg(0, "Operator needs to be converted as ANDs ORs and NOTs\n"
                       "The following code is not reasonable to do");
            
# if 0 
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_operator_conversion_aux(child, in, out);
            }
            ast_node_stack_push(out, node);
#endif
        }
    } else {
        ast_node_stack_push( out, node );
    }
}

void
dpll_operator_conversion ( struct ast *ast,
                           struct ast_node_stack *out )
{
    dpll_operator_conversion_aux(ast_end(ast) - 1, ast, out);
}



// ~(F | G) == (~F & ~G)                    // legge di De Morgan
// ~(F & G) == (~F | ~G)                    // legge di De Morgan
// Move nots inwards down the ast
void
dpll_demorgan_aux ( struct ast_node *expr_node,
                    struct ast *in,
                    struct ast_node_stack *out )
{
    struct ast_node *node = expr_node;
    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        if (node->op == OPERATOR_NOT ) {
            struct ast_node *next_node = ast_get_operand_node( in, node, 1);
            if (next_node->type == AST_NODE_TYPE_OPERATOR ) {
                if (next_node->op == OPERATOR_OR || next_node-> op == OPERATOR_AND) {
                    struct ast_node *op1_node = ast_get_operand_node( in, next_node, 1);
                    struct ast_node *op2_node = ast_get_operand_node( in, next_node, 2);
                    // ... first operand F
                    dpll_demorgan_aux(op1_node, in, out);
                    ast_node_stack_push(out, & NEGATE_NODE);
                    dpll_demorgan_aux(op2_node, in, out);
                    // ... second operand G
                    ast_node_stack_push(out, & NEGATE_NODE);
                    if (next_node->op == OPERATOR_OR) {
                        ast_node_stack_push(out, & AND_NODE);
                    } else if (next_node->op == OPERATOR_AND) {
                        ast_node_stack_push(out, & OR_NODE);
                    } else { invalid_code_path(); }
                } else {
                    // The `NODE` that follows the NOT it's a different operator from {AND, OR}
                    if (next_node->op == OPERATOR_NOT ) {
                        struct ast_node *op1_node = ast_get_operand_node( in, next_node, 1);
                        dpll_demorgan_aux(op1_node, in, out);
                        // NOTE: Do not push the negation now, DOUBLE NEGATION cancels out
                    } else {
                        // Every operator should be converted to AND's OR's NOT's or constants
                        invalid_code_path();
                    }
                }
                                                            
            } else {
                // The `NODE` that follows the NOT it's not an operator -> it's an identifier
                // Push the identifier back and the not
                ast_node_stack_push(out, next_node);
                ast_node_stack_push(out, & NEGATE_NODE);
            }
        } else {
            // Top Level operator is not a negation
            assert((node->type == AST_NODE_TYPE_OPERATOR || node->type == AST_NODE_TYPE_IDENTIFIER));
            if (node->type == AST_NODE_TYPE_OPERATOR) {
                assert_msg(node->op == OPERATOR_AND || node->op == OPERATOR_OR,
                    "Before Demogan applies every opreator should be converted to NOTS ORS ANDS");
            }
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_demorgan_aux(child, in, out);
            }
            ast_node_stack_push(out, node);
        }
    } else {
        // Top `NODE` is an identifier
        ast_node_stack_push(out, node);                
    }
}

void
dpll_demorgan ( struct ast *ast,
                struct ast_node_stack *out )
{
    dpll_demorgan_aux(ast_end(ast) - 1, ast, out);
}


void
dpll_double_negation_elimination_aux ( struct ast_node *expr_node,
                                       struct ast *in,
                                       struct ast_node_stack *out )
{
    struct ast_node *node = expr_node;
    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        if (node->op == OPERATOR_NOT ) {
            struct ast_node *next_node = ast_get_operand_node( in, node, 1);
            if (next_node->type == AST_NODE_TYPE_OPERATOR ) {
                if (next_node->op == OPERATOR_NOT ) {
                    struct ast_node *op1_node = ast_get_operand_node( in, next_node, 1);
                    dpll_double_negation_elimination_aux(op1_node, in, out);
                    // NOTE: Do not push the negation now, DOUBLE NEGATION cancels out
                } else {
                    // After applying the DEMORGAN semplification the operators `not`
                    // should garanteed to be the lasts operator, they can only precedes
                    // identifier if they not preeced other `nots`
                    assert(next_node->type == AST_NODE_TYPE_IDENTIFIER);
                }
            } else {
                // The `NODE` that follows the NOT it's not an operator -> it's an identifier
                // Push the identifier back and the not
                ast_node_stack_push(out, next_node);
                ast_node_stack_push(out, & NEGATE_NODE);
            }
        } else {
            // Top Level operator is not a negation
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_double_negation_elimination_aux(child, in, out);
            }
            ast_node_stack_push(out, node);
        }
    } else {
        // Top `NODE` is an identifier
        ast_node_stack_push(out, node);                
    }
}

void
dpll_double_negation_elimination ( struct ast *ast,
                                   struct ast_node_stack *out )
{
    dpll_double_negation_elimination_aux(ast_end(ast) - 1, ast, out);
}


// Distribute ORs inwards over ANDs: repeatedly replace
//        P ∨ ( Q ∧ R )    with    ( P ∨ Q ) ∧ ( P ∨ R ).
//        ( Q ∧ R ) ∨ P    with    ( P ∨ Q ) ∧ ( P ∨ R ).



//  ( b ∧ a ) ∨ s    with    ( s ∨ b ) ∧ ( s ∨ a )
void
dpll_or_distribution_aux ( struct ast_node *expr_node,
                           struct ast *in,
                           struct ast_node_stack *out,
                           bool *did_work)
{
    struct ast_node *node = expr_node;

    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        if (node->op == OPERATOR_OR ) {
            struct ast_node *or_op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *or_op2_node = ast_get_operand_node( in, node, 2);
            if ((or_op2_node->type == AST_NODE_TYPE_OPERATOR && or_op2_node->op == OPERATOR_AND)
                || (or_op1_node->type == AST_NODE_TYPE_OPERATOR && or_op1_node->op == OPERATOR_AND)) {
                struct ast_node *p_node  = (or_op2_node->type == AST_NODE_TYPE_OPERATOR && or_op2_node->op == OPERATOR_AND)
                    ? or_op1_node : or_op2_node;
                struct ast_node *and_node = (or_op2_node->type == AST_NODE_TYPE_OPERATOR && or_op2_node->op == OPERATOR_AND)
                    ? or_op2_node : or_op1_node;
                struct ast_node *q_node = ast_get_operand_node( in, and_node, 1);
                struct ast_node *r_node = ast_get_operand_node( in, and_node, 2);
                    
                dpll_or_distribution_aux(p_node, in, out, did_work);
                dpll_or_distribution_aux(q_node, in, out, did_work);
                ast_node_stack_push(out, & OR_NODE);
                dpll_or_distribution_aux(p_node, in, out, did_work);
                dpll_or_distribution_aux(r_node, in, out, did_work);
                ast_node_stack_push(out, & OR_NODE);
                ast_node_stack_push(out, & AND_NODE);
                *did_work = true;
            } else {
                // The or is not followed by an and op



                /* /LEZZO/ */
                dpll_or_distribution_aux(or_op1_node, in, out, did_work);
                dpll_or_distribution_aux(or_op2_node, in, out, did_work);
                ast_node_stack_push(out, node);
            }
        } else {
            // Top Level operator is not an OR `operator`
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_or_distribution_aux(child, in, out, did_work);
            }
            ast_node_stack_push(out, node);
        }
    } else {
        // Top `NODE` is an identifier
        ast_node_stack_push(out, node);                
    }
}


bool
dpll_or_distribution ( struct ast *ast,
                       struct ast_node_stack *out )
{
    bool result = false;
    dpll_or_distribution_aux(ast_end(ast) - 1, ast, out, & result);
    return result;
}


void
ast_node_stack_dump_reversed(struct ast_node_stack *stack,
                             struct ast *ast)
{
    ast_reset(ast);
    for ( struct ast_node *node = ast_node_stack_begin(stack);
          node < ast_node_stack_end(stack);
          node ++ ) {
        // printf("Dumping %.*s\n", node->text_len, node->text);
        ast_push(ast, node);
    }    
}

void
ast_node_stack_dump(struct ast_node_stack *stack,
                    struct ast *ast)
{
    ast_reset(ast);
    for ( struct ast_node *node = ast_node_stack_end(stack) - 1;
          node >= ast_node_stack_begin(stack);
          node -- ) {
        //printf("Dumping %.*s\n", node->text_len, node->text);
        ast_push(ast, node);
    }    
}

#define DPLL_TEST_C_IMPL
#include "dpll-test.c"


struct ast
dpll_convert_cnf( struct interpreter *intpt,
                  struct ast *ast )
{
    printf("DPLL Convert CNF Debug: ### \n");

    struct ast_node_stack stack = ast_node_stack_create();
    struct ast result = ast_dup( ast);

    printf("#  Starting Operators Conversion #########\n");
    dpll_operator_conversion(& result, & stack);
    ast_node_stack_dump_reversed( &stack, & result);
    ast_dbglog(& result);
    test_dpll_operator_conversion_invariant(& result);
    
    printf("#  Starting demorgan   ##########\n");
    ast_node_stack_reset(& stack);
    dpll_demorgan(& result, & stack );
    ast_node_stack_dump_reversed( &stack, & result);
    ast_dbglog(& result);
    test_dpll_demorgan_invariant(& result);



    printf("#  Starting double negation elimination   ##########\n");
    ast_node_stack_reset(& stack);
    dpll_double_negation_elimination(& result, & stack );
    ast_node_stack_dump_reversed( &stack, & result);
    ast_dbglog(& result);
    test_dpll_double_negation_elimination_invariant(& result);
        

    // replace
    //    •  `¬(∀x  P(x))`  with  `∃x  ¬P(x)`
    //    •  `¬(∃x  P(x))`  with  `∀x  ¬P(x)`
    // After that, a ¬ may occur only immediately before a predicate symbol.
    
    // .......... IMPLEMENTATION FOR PROPOSITIONAL CALCULUS HERE .........................

    // Distribute ORs inwards over ANDs: repeatedly replace P ∨ ( Q ∧ R ) with ( P ∨ Q ) ∧ ( P ∨ R ).
    printf("#  Starting Or Distribution   ##########\n");
    ast_node_stack_reset(& stack);
    while (dpll_or_distribution(& result, & stack )) {
        ast_node_stack_dump_reversed( &stack, & result);
        ast_node_stack_reset(& stack);
    }
    ast_node_stack_dump_reversed( &stack, & result);
    ast_dbglog(& result);
    test_dpll_or_distribution_invariant(& result);
    
    
    ast_node_stack_reset(& stack);
    printf("DPLL Convert CNF Debug: ### END ### \n");
    
    return result;
}





static inline bool
dpll_is_empty_clause( struct interpreter *intpt,
                      struct ast *cnf )
{
    return ( ast_num_nodes(ast) == 0);
}


// DEF: A formula is consistent if it's true.
//      A formula which is NOT consistent it is NOT necessary false.
/*    if Φ is a consistent set of literals */
/*        then return true; */

bool
dpll_is_consistent( struct interpreter *intpt,
                    struct ast *cnf )
{
    struct symtable *symtable = & intpt->symtable;
    bool result = false;
    struct ast_node *node = ast_peek_addr(cnf);
    if (node == NULL) {
        return false;
    }
    
    if ( ast_node_is_operator(node) ) {
        if ( node->op == OPERATOR_NOT) {
            struct ast_node *child_node = ast_get_operand_node( cnf, node, 1);
            assert(child_node);
            if ( child_node->type == AST_NODE_TYPE_CONSTANT) {
                bool v = ast_node_constant_to_bool( node );
                result = !v;
            } else {
                // Negation can only precedes constants or identifiers
                // Negation Followed by Negation should be deleted
                // from the double negation elimination step
                assert(child_node->type == AST_NODE_TYPE_IDENTIFIER);
                struct symbol_info *syminfo = symtable_get_syminfo( symtable,
                                                                    child_node->text,
                                                                    child_node->text_len);
                if ( syminfo->has_value_assigned ) {
                    result = !(syminfo->value);
                } else {
                    // Don't know
                    result = false;
                }
            }
        } else {
            assert(node->op == OPERATOR_AND || node->op == OPERATOR_OR);
            result = false;
        }
    } else if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
        struct symbol_info *syminfo = symtable_get_syminfo( symtable,
                                                            node->text,
                                                            node->text_len);
        if ( syminfo->has_value_assigned ) {
            result = !(syminfo->value);
        } else {
            // Don't know
            result = false;
        }

    } else if ( node->type == AST_NODE_TYPE_CONSTANT ) {
        bool v = ast_node_constant_to_bool( node );
        result = v;
    } else if ( node->type == AST_NODE_TYPE_DELIMITER ) {
        assert_msg(0, "Needs testing if formula contains delimiter");
    } else {
        invalid_code_path();
    }
    
    // If gone trough the entire iteration, the result is true
    return result;
}

/*
    USAGE:
    struct ast_node *node = NULL; // <--- NULL Initialization is important to determine first call
    while ( dpll_next_unit_clause(intpt, cnf, &node) ) { ... }

    OR:
    struct ast_node *node = NULL; // <--- NULL Initialization is important to determine first call
    while (node = dpll_next_unit_clause(intpt, cnf, & node ) { .... }
*/
struct ast_node *
dpll_next_unit_clause( struct interpreter *intpt,
                       struct ast *cnf,
                       struct ast_node **node,
                       bool *is_negated )
{

    return false;
}


bool
dpll_eval(struct interpreter *intpt,
          struct ast *cnf)
{
    struct symtable *symtable = & intpt->symtable;
    struct vm_stack *vms = & intpt->vms;

    bool result = false;
    for (struct ast_node *node = ast_begin(cnf);
         node < ast_end(cnf);
         node ++ ) {
        if ( node->type == AST_NODE_TYPE_IDENTIFIER  || node->type == AST_NODE_TYPE_CONSTANT) {
            bool value;
            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                struct symbol_info *syminfo = symtable_get_syminfo(symtable, node->text, node->text_len);
                assert(syminfo->has_value_assigned);
                value = syminfo->value;
            } else if ( node->type == AST_NODE_TYPE_CONSTANT ) {
                value = ast_node_constant_to_bool(node);
            }
            vm_stack_push( vms, value );
        } else {
            // Token is an operator: Needs to perform the operation
            //                       and push it into the stack
            eval_operator( node, vms ) ;
        }            
    }
    return result;
}

// Algorithm for on the AST Symbols,
// for every symbols that appear in a unit clause
// return that it is a unit clause and go and assign a value to it

// input in the ast there should be a formula
// of this kind: c1 & c2 & c3 & c4
// Where ci denotes the i-th unit clause (a or ~a) and `&` the `AND` operator
// ---------------------------------------------------------
/* Algorithm DPLL */
/*   Input: A set of clauses Φ. */
/*   Output: A Truth Value. */
// --------------------------------------------------------
/* function DPLL(Φ) */




void
dpll_preprocess ( struct interpreter *intpt,
                  struct ast         *cnf)
{
}

bool
dpll_is_pure_literal( struct interpreter *intpt,
                      struct ast *cnf,
                      struct ast_node *node,
                      bool *appears_negated )
{
    return false;
}




bool
dpll_solve_recurse(struct interpreter *intpt,
                   struct ast *cnf)
{
    bool result = false;
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;

    // DEF: A formula is consistent if it's true.
    //      A formula which is NOT consistent it is NOT necessary false.
    /*    if Φ is a consistent set of literals */
    /*        then return true; */
    if ( dpll_is_consistent(intpt, cnf))
        return true;

    //    AST Has been reduced to 0 nodes
    /*    if Φ contains an empty clause */
    /*        then return false; */
    if (dpll_is_empty_clause(intpt, cnf))
        return false;

    
    
    return result;
}



void
dpll_solve(struct interpreter *intpt,
           struct ast *ast)
{
    dpll_preprocess(intpt, ast);
    struct ast cnf = dpll_convert_cnf( intpt,ast );


    dpll_solve_recurse(intpt, & cnf);


CLEANUP:
    ast_clear(& cnf);
}














#if 0
// input in the ast there should be a formula
// of this kind: c1 & c2 & c3 & c4
// Where ci denotes the i-th unit clause (a or ~a) and `&` the `AND` operator
// ---------------------------------------------------------
/* Algorithm DPLL */
/*   Input: A set of clauses Φ. */
/*   Output: A Truth Value. */
// --------------------------------------------------------
/* function DPLL(Φ) */
bool
__old__dpll_solve_recurse(struct interpreter *intpt,
                          struct ast *cnf)
{
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    intpt_info_printf(intpt, "Input to the dpll solver is:\n\t");
    ast_representation_dbglog(intpt);


    assert_msg(0, "Needs implementation for the propagation of the AST"
               " It needs to recompact or choose a deferred less recursive aproach");
    

//  I still do not understand this one
    // Verifies validity of the formula either by solving it.
    // or by waiting for the recursion to create an ast
    // with a formula containing a `true` or `false` constant.
    // DEF: A formula is consistent if it's true.
    //      A formula which is NOT consistent it is NOT necessary false.
/*    if Φ is a consistent set of literals */
/*        then return true; */
    if ( dpll_is_consistent(intpt, cnf) )
        return true;
    
//    AST Has been reduced to 0 nodes
/*    if Φ contains an empty clause */
/*        then return false; */
    if ( dpll_is_empty_clause(intpt, cnf))
        return false;

# if 0
    // Substitute with a straight solve request if every identifier is assigned
    // watchout for meta-constant operators like `in` and alikes.
#endif
    
    /* Optimization possibility:
       ===================================
       while we iterate over the AST to check the presence of unit clauses
       we can test if the formula is computable by keeping a flag and `orring` and
       `clearing` it based on the fact that every identifier has a value assigned
       or not.
     */
    
// Literal atomic formula or its negation: any symbol and constants
// a unit clause: clauses that are composed of a single literal.
// Because each clause needs to be satisfied, we know that this literal must be true
/*    for every unit clause {l} in Φ */
/*       Φ ← unit-propagate(l, Φ); */
    struct ast_node *node = NULL;
    bool is_negated = false;
    while ( dpll_next_unit_clause(intpt, cnf, &node, & is_negated) ) {
        const bool value =  ! is_negated;
        ast_node_convert_to_constant(node, value);
        // Now handle the propagation, not really necessary to rebuild
        // the ast it can be deferred somewhere else.
    }
        


    // A Pure literal is any literal that does not appear with its' negation in the formula
    // WIKIPEDIA DEF: In the context of a formula in the
    //                conjunctive normal form, a literal is pure if the literal's
    //                complement does not appear in the formula.
/*    for every literal l that occurs pure in Φ */
/*       Φ ← pure-literal-assign(l, Φ); */
    {
        for (struct ast_node *node = ast_begin(cnf);
             node < ast_end(cnf);
             node ++) {

            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                bool appears_negated;
                bool is_pure = dpll_is_pure_literal( intpt, cnf, node, & appears_negated );
                if (is_pure) {
                    const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                }
            }
            
        }
    }

    bool result = false;
    /*    l ← choose-literal(Φ); */ // A CASO !
    /*    return DPLL(Φ ∧ {l}) or DPLL(Φ ∧ {not(l)}); */
    {
        struct ast_node *pick = ast_begin(cnf);
        for ( ;
             pick < ast_end(cnf);
             pick ++ ) {
            if ( pick->type == AST_NODE_TYPE_IDENTIFIER ) {
                break;
            }
        }
        assert(pick);
        
        for (struct ast_node *node = ast_begin(cnf);
             node < ast_end(cnf);
             node ++ ) {
            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                if ( strncmp(pick->text, node->text, MIN(pick->text_len, node->text_len)) == 0 ) {
                    const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                }
            }
        }


        result = dpll_solve_recurse(intpt, cnf);

        if ( !result ) { // short circuit optimization 
            for (struct ast_node *node = ast_begin(cnf);
                 node < ast_end(cnf);
                 node ++ ) {
                if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                    if ( strncmp(pick->text, node->text, MIN(pick->text_len, node->text_len)) == 0 ) {
                        const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                    }
                }
            }
            result |= dpll_solve_recurse(intpt, cnf);
        }
    }

    return result;
}
#endif






#endif /* DPLL_C_IMPL */
