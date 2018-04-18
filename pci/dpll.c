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
    ast_for ( it, *clauses_ast, node ) {
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


// Algorithm for on the AST Symbols,
// for every symbols that appear in a unit clause
// return that it is a unit clause and go and assign a value to it

struct ast_node *
dpll_next_unit_clause_symbol ( struct interpreter *intpt,
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
                 n->type == AST_NODE_TYPE_CONSTANT) {
                bool v1;
                bool has_v1 = ast_node_value_assigned(& intpt->symtable, n, & v1);
                if ( ! has_v1 ) {
                    *is_negated = false;
                    result  = n;
                    break;
                }
            } else if (n->type == AST_NODE_TYPE_OPERATOR) {
                if (n->op == OPERATOR_NEGATE) {
                    size_t first_operand = ast_get_operand_index( clauses_ast,
                                                                  n - clauses_ast->nodes, 1);
                    struct ast_node *n1 = & clauses_ast->nodes[first_operand];
                    if ( n1->type == AST_NODE_TYPE_OPERATOR) {
                        bool v1;
                        bool has_v1 = ast_node_value_assigned(& intpt->symtable, n, & v1);
                        if ( ! has_v1 ) {
                            *is_negated = true;
                            result  = n;
                            break;
                        }
                    }
                } else if ( n->op == OPERATOR_AND ) {
                    // Top level ANDS are always unit_clauses
                    size_t second_operand = ast_get_operand_index( clauses_ast,
                                                                   n - clauses_ast->nodes, 2);
                    size_t first_operand = ast_get_operand_index( clauses_ast,
                                                                  n - clauses_ast->nodes, 1);

                    struct ast_node *n2 = & clauses_ast->nodes[second_operand];
                    struct ast_node *n1 = & clauses_ast->nodes[first_operand];
                    // Il primo e' identifier e non assegnato e il secondo e' non assegnato oppure identifier
                    if (n1->type == AST_NODE_TYPE_IDENTIFIER ) {
                        bool v1;
                        bool has_v1 = ast_node_value_assigned(& intpt->symtable, n1, & v1);
                        if ( ! has_v1 ) {
                            result  = n;
                            break;
                        }
                    } else if (n2->type == AST_NODE_TYPE_IDENTIFIER ) {
                        bool v2;
                        bool has_v2 = ast_node_value_assigned( & intpt->symtable, n2, &v2);
                        if (!has_v2 ) {
                            result = n;
                            break;
                        }
                    }
                } else {
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
    size_t it = 0;
    struct ast_node *node = NULL;
    ast_for ( it, *clauses_ast, node ) {
        struct symbol_info *syminfo = symtable_get_syminfo(& intpt->symtable,
                                                           node->text, node->text_len);
        bool v = syminfo->value, hv = syminfo->has_value_assigned;
        // Important in this { if / else if / else } match the constants & identifiers first.
        if (node->type == AST_NODE_TYPE_CONSTANT ) {
            node->num_arguments = 0;     // No functional dependence
        } else if (node->type == AST_NODE_TYPE_IDENTIFIER) {
            if ( !hv ) {
                node->num_arguments = 1; // Functionally depends on himself
            } else {
                node->num_arguments = 0; // No functional dependence
            }
        } else if (node->type == AST_NODE_TYPE_OPERATOR) {
            size_t num_arguments = 0;
            size_t num_operands = operator_num_operands(node);
            for (size_t operand = 1; operand <= num_operands; operand++) {
                size_t oi = ast_get_operand_index(clauses_ast, node - clauses_ast->nodes, operand);
                num_arguments += clauses_ast->nodes[oi].num_arguments;
            }
            node->num_arguments = num_arguments;
        } else {
            invalid_code_path();
        }
    }
}

struct ast_node *
dpll_next_unit_clause2 ( struct interpreter *intpt,
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
        }
    }

    *node = result;
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
