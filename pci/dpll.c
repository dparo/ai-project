#ifndef DPLL_C_INCLUDE
#define DPLL_C_INCLUDE


//#######################################################
#endif /* DPLL_C_INCLUDE */
#if !defined DPLL_C_IMPLEMENTED && defined DPLL_C_IMPL
#define DPLL_C_IMPLEMENTED
//#######################################################



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
    assert(node);
    struct ast_node *result = NULL;
    struct ast_node *start = NULL;
    
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
    
    assert(start);
    if ( !((start >= clauses_ast->nodes)
           && (start < & clauses_ast->nodes[clauses_ast->num_nodes]))) {
        // Not in bounds
        *node = NULL;
        return NULL;
    } else {
        struct ast_node *n = start;
        for ( n = start; n >= clauses_ast->nodes; n-- ) {
            if ( n->type == AST_NODE_TYPE_IDENTIFIER ||
                 n->type == AST_NODE_TYPE_CONSTANT ||
                 (n->type == AST_NODE_TYPE_OPERATOR && n->op == OPERATOR_NEGATE)) {
                     if (n->type == AST_NODE_TYPE_OPERATOR && n->op == OPERATOR_NEGATE) {
                         n--;
                         assert_msg(n >= clauses_ast->nodes, "Malformed formula");
                         if (n->type == AST_NODE_TYPE_IDENTIFIER || n->type == AST_NODE_TYPE_CONSTANT) {
                             *is_negated = true;
                             result = n;
                             break;
                         } else {
                             // skip childs
                             size_t operator_index = n - clauses_ast->nodes;
                             size_t first_operand_index = ast_get_operand_index( clauses_ast, operator_index, 1);
                             n = (& clauses_ast->nodes[first_operand_index]);
                         }
                     } else {
                         // return this
                         *is_negated = false;
                         result = n;
                         break;

                     }
            } else if (n->type == AST_NODE_TYPE_OPERATOR && n->op != OPERATOR_NEGATE) {
                if ( n->op != OPERATOR_AND ) {
                    // skip childs
                    size_t operator_index = n - clauses_ast->nodes;
                    size_t first_operand_index = ast_get_operand_index( clauses_ast, operator_index, 1);
                    n = (& clauses_ast->nodes[first_operand_index]);
                }
            } else {
                assert_msg(0, "Invalid code path for now");
            }
        }
    }

    *node = result;
    return result;
    
}


bool
dpll_is_pure_literal( struct interpreter *intpt,
                      struct ast *clauses_ast,
                      struct ast_node *node,
                      bool *appears_negated )
{
    assert(node);
    bool result = true;
    bool found_negated = false;
    bool found_normal = false;

    assert(clauses_ast->num_nodes > 0);

    struct ast_node *n;
    for ( n = & clauses_ast->nodes[clauses_ast->num_nodes - 1];
          n >= clauses_ast->nodes;
          n-- ) {
        if (n->type == AST_NODE_TYPE_OPERATOR && n->op == OPERATOR_NEGATE) {
            n--;
            assert_msg(n >= clauses_ast->nodes, "Malformed formula");
            if (n->type == AST_NODE_TYPE_IDENTIFIER || n->type == AST_NODE_TYPE_CONSTANT) {
                if ( strncmp(n->text, node->text, MIN(node->text_len, n->text_len)) == 0 ) {
                    found_negated = true;
                    if ( found_negated && found_normal ) { result = false; break; }
                }

            } else {
                // skip childs
                size_t operator_index = n - clauses_ast->nodes;
                size_t first_operand_index = ast_get_operand_index( clauses_ast, operator_index, 1);
                n = (& clauses_ast->nodes[first_operand_index]);
            }
        } else if (n->type == AST_NODE_TYPE_IDENTIFIER || n->type == AST_NODE_TYPE_CONSTANT) {
            if ( strncmp(n->text, node->text, MIN(node->text_len, n->text_len)) == 0 ) {
                found_normal = true;
                if ( found_negated && found_normal ) { result = false; break; }
            // return this
            }            
        }
    }
    if (result) {
        *appears_negated = found_negated;
    }
    return result;
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
        struct ast_node *node = NULL;
        size_t it = 0;
        ast_for(it, *clauses_ast, node) {
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
        struct ast_node *pick = NULL;
        size_t it = 0;
        ast_for(it, *clauses_ast, pick) {
            if ( pick->type == AST_NODE_TYPE_IDENTIFIER ) {
                break;
            }
        }
        assert(pick);
        
        struct ast_node *node = NULL;
        ast_for(it, *clauses_ast, node) {
            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                if ( strncmp(pick->text, node->text, MIN(pick->text_len, node->text_len)) == 0 ) {
                    const bool value = 1;
                    ast_node_convert_to_constant(node, value);
                }
            }
        }


        result = dpll_solve(intpt, clauses_ast);

        if ( !result ) { // short circuit optimization 
            ast_for(it, *clauses_ast, node) {
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
