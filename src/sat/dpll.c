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
        if (node->op == OPERATOR_DOUBLE_IMPLY
            || node->op == OPERATOR_IMPLY ) {
            struct ast_node *op1_node = ast_get_operand_node( in, node, 1);
            struct ast_node *op2_node = ast_get_operand_node (in, node, 2);

            if (node->op == OPERATOR_DOUBLE_IMPLY) {
                dpll_operator_conversion_aux(op1_node, in, out);
                dpll_operator_conversion_aux(op2_node, in, out);
                ast_node_stack_push(out, & NEGATE_NODE);
                ast_node_stack_push(out, & OR_NODE);
                dpll_operator_conversion_aux(op1_node, in, out);
                ast_node_stack_push(out, & NEGATE_NODE);
                dpll_operator_conversion_aux(op2_node, in, out);
                ast_node_stack_push(out, & OR_NODE);
                ast_node_stack_push(out, & AND_NODE);

            } else if (node->op == OPERATOR_IMPLY) {
                dpll_operator_conversion_aux(op1_node, in, out);
                ast_node_stack_push(out, & NEGATE_NODE);
                dpll_operator_conversion_aux(op2_node, in, out);
                ast_node_stack_push(out, & OR_NODE);
            } 
        } else {
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_operator_conversion_aux(child, in, out);
            }
            ast_node_stack_push(out, node);
        }
    }  else {
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
        if (node->op == OPERATOR_NEGATE ) {
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
                    if (next_node->op == OPERATOR_NEGATE ) {
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
        if (node->op == OPERATOR_NEGATE ) {
            struct ast_node *next_node = ast_get_operand_node( in, node, 1);
            if (next_node->type == AST_NODE_TYPE_OPERATOR ) {
                if (next_node->op == OPERATOR_NEGATE ) {
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
void
dpll_or_distribute_aux ( struct ast_node *expr_node,
                         struct ast *in,
                         struct ast_node_stack *out )
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
                    
                dpll_or_distribute_aux(p_node, in, out);
                dpll_or_distribute_aux(q_node, in, out);
                ast_node_stack_push(out, & OR_NODE);
                dpll_or_distribute_aux(p_node, in, out);
                dpll_or_distribute_aux(r_node, in, out);
                ast_node_stack_push(out, & OR_NODE);
                ast_node_stack_push(out, & AND_NODE);
            } else {
                // The or is not followed by an and op



                /* /LEZZO/ */
                dpll_or_distribute_aux(or_op1_node, in, out);
                dpll_or_distribute_aux(or_op2_node, in, out);
                ast_node_stack_push(out, node);
            }
        } else {
            // Top Level operator is not an OR `operator`
            uint numofoperands = operator_num_operands(node);
            for( size_t operand_num = 1;
                 operand_num <= numofoperands;
                 operand_num++ ) {
                struct ast_node *child = ast_get_operand_node(in, node, operand_num);
                dpll_or_distribute_aux(child, in, out);
            }
            ast_node_stack_push(out, node);
        }
    } else {
        // Top `NODE` is an identifier
        ast_node_stack_push(out, node);                
    }
}


void
dpll_or_distribute ( struct ast *ast,
                     struct ast_node_stack *out )
{
    dpll_or_distribute_aux(ast_end(ast) - 1, ast, out);
}


void
ast_node_stack_dump_reversed(struct ast_node_stack *stack,
                             struct ast *ast)
{
    ast_reset(ast);
    for ( struct ast_node *node = ast_node_stack_begin(stack);
          node < ast_node_stack_end(stack);
          node ++ ) {
        printf("Dumping %.*s\n", node->text_len, node->text);
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
        printf("Dumping %.*s\n", node->text_len, node->text);
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

    printf("#  Starting Implication elimination #########\n");
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
    dpll_or_distribute(& result, & stack );
    ast_node_stack_dump_reversed( &stack, & result);
    ast_dbglog(& result);
    test_dpll_or_distribute_invariant(& result);
    
    
    ast_node_stack_reset(& stack);
    printf("DPLL Convert CNF Debug: ### END ### \n");
    
    return result;
}





bool
dpll_is_empty_clause( struct interpreter *intpt,
                      struct ast *clauses_ast )
{
    bool result = true;

    for ( struct ast_node *node = ast_begin(clauses_ast);
          node < ast_end(clauses_ast);
          node ++ ) {
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


    for (struct ast_node *node = ast_begin(clauses_ast);
         node < ast_end(clauses_ast);
         node ++ ) {
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

/*
    USAGE:
    struct ast_node *node = NULL; // <--- NULL Initialization is important to determine first call
    while ( dpll_next_unit_clause(intpt, clauses_ast, &node) ) { ... }

    OR:
    struct ast_node *node = NULL; // <--- NULL Initialization is important to determine first call
    while (node = dpll_next_unit_clause(intpt, clauses_ast, & node ) { .... }
*/
struct ast_node *
dpll_next_unit_clause( struct interpreter *intpt,
                       struct ast *clauses_ast,
                       struct ast_node **node,
                       bool *is_negated )
{

    return false;
}


// Algorithm for on the AST Symbols,
// for every symbols that appear in a unit clause
// return that it is a unit clause and go and assign a value to it




// let's supose i have an AST
// let's mark the operators as explored

// It's the operator that determines if the childs can be unit clauses
// preprocess stage,

// a   // its obviously a unit clause
// a & b // a, b are both unit clauses
// a & ( b | c)  // only a is a unit clause if b and c theu haven't got any assigned value
// a & ( b | c)  // once c or b is assigned the remaining operand becomes a unit clause
// so recapping to know if a node is a unit clause depends on the parent,
//    we need to traverse the ast entirely from the top
// a node can be a unit clause for the subtree to where it belongs
// extending the tree with an operator -> a can remain a unit clause or not
//   if the subtree gets extended with an AND the childs remains unit clauses
//   otherwise if the ast gets extended with an OR statement or any other statement

// Note a unit clause means that a subtree is a function dependent on one input only !
// Note a constant is a subtree that does not depend on any input thus its value is determined

// NOTE: This formula (a | a) the `OR` operator will have a num_arguments = 2
// Even thought techically the dependence it's only on 1 argument
void
dpll_preprocess ( struct interpreter *intpt,
                  struct ast         *clauses_ast)
{
    for (struct ast_node *node = ast_begin(clauses_ast);
         node < ast_end(clauses_ast);
         node ++ ) {
        // Important in this { if / else if / else } match the constants & identifiers first.
        if (node->type == AST_NODE_TYPE_CONSTANT ) {
            node->num_args = 0;     // No functional dependence
        } else if (node->type == AST_NODE_TYPE_IDENTIFIER) {
            struct symbol_info *syminfo = symtable_get_syminfo(& intpt->symtable,
                                                               node->text, node->text_len);
            bool v = syminfo->value, hv = syminfo->has_value_assigned;
            if ( !hv ) {
                node->num_args = 1; // Functionally depends on himself
            } else {
                node->num_args = 0; // No functional dependence
            }
        } else if (node->type == AST_NODE_TYPE_OPERATOR) {
            size_t num_args = 0;
            size_t num_operands = operator_num_operands(node);
            for (size_t operand = 1; operand <= num_operands; operand++) {
                struct ast_node *op_node =
                    ast_get_operand_node(clauses_ast, node, operand);
                num_args += node->num_args;
            }
            node->num_args = num_args;
        } else {
            invalid_code_path();
        }
    }
}

bool
dpll_is_pure_literal( struct interpreter *intpt,
                      struct ast *clauses_ast,
                      struct ast_node *node,
                      bool *appears_negated )
{
    return false;
}


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
dpll_solve(struct interpreter *intpt,
           struct ast *clauses_ast)
{
    struct symtable *symtable = & intpt->symtable;
    struct vm_inputs *vmi = & intpt->vmi;
    intpt_info_printf(intpt, "Input to the dpll solver is:\n\t");
    ast_representation_dbglog(intpt);


    assert_msg(0, "Needs implementation for the propagation of the AST"
               " It needs to recompact or choose a deferred less recursive aproach");

    dpll_preprocess(intpt, clauses_ast);
    

//  I still do not understand this one
    // Verifies validity of the formula either by solving it.
    // or by waiting for the recursion to create an ast
    // with a formula containing a `true` or `false` constant.
    // DEF: A formula is consistent if it's true.
    //      A formula which is NOT consistent it is NOT necessary false.
/*    if Φ is a consistent set of literals */
/*        then return true; */
    if ( dpll_is_consistent(intpt, clauses_ast) )
        return true;
    
//    AST Has been reduced to 0 nodes
/*    if Φ contains an empty clause */
/*        then return false; */
    if ( dpll_is_empty_clause(intpt, clauses_ast))
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
    while ( dpll_next_unit_clause(intpt, clauses_ast, &node, & is_negated) ) {
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
        for (struct ast_node *node = ast_begin(clauses_ast);
             node < ast_end(clauses_ast);
             node ++) {

            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                bool appears_negated;
                bool is_pure = dpll_is_pure_literal( intpt, clauses_ast, node, & appears_negated );
                if (is_pure) {
                    const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                }
            }
            
        }
    }

    bool result = false;
    /*    l ← choose-literal(Φ); */
    /*    return DPLL(Φ ∧ {l}) or DPLL(Φ ∧ {not(l)}); */
    {
        struct ast_node *pick = ast_begin(clauses_ast);
        for ( ;
             pick < ast_end(clauses_ast);
             pick ++ ) {
            if ( pick->type == AST_NODE_TYPE_IDENTIFIER ) {
                break;
            }
        }
        assert(pick);
        
        for (struct ast_node *node = ast_begin(clauses_ast);
             node < ast_end(clauses_ast);
             node ++ ) {
            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                if ( strncmp(pick->text, node->text, MIN(pick->text_len, node->text_len)) == 0 ) {
                    const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                }
            }
        }


        result = dpll_solve(intpt, clauses_ast);

        if ( !result ) { // short circuit optimization 
            for (struct ast_node *node = ast_begin(clauses_ast);
                 node < ast_end(clauses_ast);
                 node ++ ) {
                if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                    if ( strncmp(pick->text, node->text, MIN(pick->text_len, node->text_len)) == 0 ) {
                        const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                    }
                }
            }
            result |= dpll_solve(intpt, clauses_ast);
        }
    }

    return result;
}


#endif /* DPLL_C_IMPL */
