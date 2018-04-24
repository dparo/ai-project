#ifndef DPLL_CNF_C_INCLUDE
#define DPLL_CNF_C_INCLUDE


//#######################################################
#endif /* DPLL_CNF_C_INCLUDE */
#if !defined DPLL_CNF_C_IMPLEMENTED && defined DPLL_CNF_C_IMPL
#define DPLL_CNF_C_IMPLEMENTED
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
        if ( node->op == OPERATOR_NOT ) {
            struct ast_node *op1_node = ast_get_operand_node(in, node, 1);
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, node);
        } else if ( node->op == OPERATOR_OR
                    || node->op == OPERATOR_AND ) {
            struct ast_node *op1_node = ast_get_operand_node(in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node(in, node, 2);
            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, node);
        } else if (node->op == OPERATOR_DOUBLE_IMPLY|| node->op == OPERATOR_EQUAL_EQUAL
            || node->op == OPERATOR_NOT_EQUAL ) {
            struct ast_node *op1_node = ast_get_operand_node(in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node(in, node, 2);

            
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
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
        } else if (node->op == OPERATOR_NOR ) {
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);
            
            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & OR_NODE);
            ast_node_stack_push(out, & NEGATE_NODE);
        } else if (node->op == OPERATOR_NAND ) {
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & AND_NODE);
            ast_node_stack_push(out, & NEGATE_NODE);
        } else if (node->op == OPERATOR_XOR) {
            // P ^ Q = (~P | ~Q) & (P | Q)           
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);
            
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
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & AND_NODE);           
        } else if (node->op == OPERATOR_GREATER_EQUAL) {
            // (a >= b) ==== (a | ~b)
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            dpll_operator_conversion_aux(op1_node, in, out);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            ast_node_stack_push(out, & OR_NODE);
        } else if (node->op == OPERATOR_LESS ) {
            // (a < b) ==== (~a & b)
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            dpll_operator_conversion_aux(op1_node, in, out);
            ast_node_stack_push(out, & NEGATE_NODE);
            dpll_operator_conversion_aux(op2_node, in, out);
            ast_node_stack_push(out, & AND_NODE);
        } else if (node->op == OPERATOR_LESS_EQUAL ) {
            // (a <= b) ==== (~a | b)
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

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

#define DPLL_CNF_TEST_C_IMPL
#include "dpll-cnf-test.c"


struct ast
dpll_convert_cnf( struct ast *ast )
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







#endif /* DPLL_CNF_C_IMPL */
