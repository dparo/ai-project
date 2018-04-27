#ifndef DPLL_C_INCLUDE
#define DPLL_C_INCLUDE


//#######################################################
#endif /* DPLL_C_INCLUDE */
#if !defined DPLL_C_IMPLEMENTED && defined DPLL_C_IMPL
#define DPLL_C_IMPLEMENTED
//#######################################################

#define DPLL_CNF_C_IMPL
#include "dpll-cnf.c"


void
dpll_test_invariant_no_value_assigned(struct ast_node *node)
{
#if __DEBUG
    struct symtable *symtable = & global_interpreter.symtable;
    struct symbol_info *syminfo = symtable_syminfo_from_node(symtable, node);
    assert(! syminfo->has_value_assigned);
#endif
}


void
dpll_identifier_assign_value(struct ast_node *node,
                             bool value)
{
    assert(node);
    assert(node->type == AST_NODE_TYPE_IDENTIFIER);
    struct symtable *symtable = & global_interpreter.symtable;
    struct symbol_info *syminfo = symtable_syminfo_from_node(symtable, node);
    assert(! syminfo->has_value_assigned);
    syminfo->has_value_assigned = true;
    syminfo->value = value;
}


bool
dpll_eval(struct ast *cnf)
{
    struct symtable *symtable = & global_interpreter.symtable;
    struct vm_stack *vms = & global_interpreter.vms;

    bool result = false;
    for (struct ast_node *node = ast_begin(cnf);
         node < ast_end(cnf);
         node ++ ) {
        if ( node->type == AST_NODE_TYPE_IDENTIFIER  || node->type == AST_NODE_TYPE_CONSTANT) {
            bool value;
            if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
                struct symbol_info *syminfo = symtable_syminfo_from_node(symtable, node);
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


static inline bool
dpll_is_empty_or_false_clause( struct ast *cnf )
{
    if (ast_num_nodes(cnf) == 0)
        return true;
    else if (ast_num_nodes(cnf) > 0) {
        return ast_node_is_false_constant( ast_end(cnf) - 1);
    }
    if (ast_num_nodes(cnf) > 1) {
        struct ast_node *node = (ast_end(cnf) - 1);
        if (node->type == AST_NODE_TYPE_OPERATOR &&
            node->op == OPERATOR_NOT) {
            struct ast_node *child_node = ast_get_operand_node( cnf, node, 1);
            if ( ast_node_is_true_constant(child_node)) {
                return true;
            }
        }
    }
    return false;
}


// DEF: A formula is consistent if it's true.
//      A formula which is NOT consistent it is NOT necessary false.
/*    if Φ is a consistent set of literals */
/*        then return true; */

bool
dpll_is_consistent( struct ast *cnf )
{
    struct symtable *symtable = & global_interpreter.symtable;
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
                bool v = ast_node_constant_to_bool( child_node );
                result = !v;
            } else {
                // Negation can only precedes constants or identifiers
                // Negation Followed by Negation should be deleted
                // from the double negation elimination step
                assert(child_node->type == AST_NODE_TYPE_IDENTIFIER);
                struct symbol_info *syminfo = symtable_syminfo_from_node( symtable,
                                                                          child_node);
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
        struct symbol_info *syminfo = symtable_syminfo_from_node( symtable, node);
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


struct ast_node *
dpll_search_parent_node( struct ast *cnf,
                         struct ast_node *child)
{
    assert(child >= ast_begin(cnf));
    struct ast_node *result = NULL;

    for( struct ast_node *node = ast_end(cnf) - 1;
         node > child;
         node -- ) {
        uint numofoperands = operator_num_operands(node);
        for( size_t operand_num = 1;
             operand_num <= numofoperands;
             operand_num++ ) {
            if (ast_get_operand_node(cnf, node, operand_num) == child) {
                // Found the father
                return node;
            }
        }

    }
    return NULL;
}

bool
dpll_is_identifier_unit_clause( struct ast *cnf,
                                struct ast_node *identifier,
                                bool *is_negated)
{
    assert(identifier->type == AST_NODE_TYPE_IDENTIFIER);
    struct ast_node *parent = dpll_search_parent_node(cnf, identifier);
    if (!parent) {
        *is_negated = false;
        return true;
    }
    
    assert(parent);
    assert(parent->type == AST_NODE_TYPE_OPERATOR);
    switch (parent->op) {
    case OPERATOR_AND: {
#    if __DEBUG
        struct ast_node *new_parent = dpll_search_parent_node(cnf, parent);
        assert(!new_parent || (new_parent->type == AST_NODE_TYPE_OPERATOR
                              && new_parent->op == OPERATOR_AND));
#    endif
        *is_negated = false;
        return true;
    } break;

    case OPERATOR_NOT: {
        struct ast_node *new_parent = dpll_search_parent_node(cnf, parent);
        if ( new_parent ) {
            if (new_parent->type == AST_NODE_TYPE_OPERATOR
                && new_parent->op == OPERATOR_OR) {
                return false;
            } else {
                assert(new_parent->type == AST_NODE_TYPE_OPERATOR
                       && new_parent->op == OPERATOR_AND);
                *is_negated = true;
                return true;
            }
        } else {
            *is_negated = true;
            return true;
        }
    } break;

    case OPERATOR_OR: {
        // The OR operator means indecision over the input
        // and needs more unit-propagation and assignments
        // to be resolved
        return false;
    } break;

    default: {
        invalid_code_path();
    } break;
    }

    return false;
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
dpll_next_unit_clause( struct ast *cnf,
                       bool *is_negated )
{
    for ( struct ast_node *node = ast_begin(cnf);
          node < ast_end(cnf);
          node++ ) {
        if (node->type == AST_NODE_TYPE_IDENTIFIER) {
            if (dpll_is_identifier_unit_clause(cnf, node, is_negated)) {
                return node;
            }
        } else if (node->type == AST_NODE_TYPE_CONSTANT) {
            if (node != ast_peek_addr(cnf)) {
                assert_msg(0, "No constants Allowed, unit_propagation should removed them");
            }
        } else if (node->type == AST_NODE_TYPE_OPERATOR) {
        }
    }
    return NULL;
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
dpll_preprocess ( struct ast *raw_ast)
{

}



void
dpll_stack_dump_into_stack ( struct ast_node_stack *dumper,
                             struct ast_node_stack *dumpee)
{
    for( struct ast_node *node = ast_node_stack_begin(dumper);
         node < ast_node_stack_end(dumper);
         node ++ ) {
        ast_node_stack_push(dumpee, node);
    }
}



// `id` Maybe `NULL`, if `id` null unit propagate
// will only merge constants, usefull as preprocess
// step. Value could be anything in this case, pass 0

// if `id` != NULL when unit_propagate finds that node
// it considers it as a constant and can do
// more agressive propagation
struct ast_node_stack
dpll_unit_propagate_recurse( struct ast *cnf,
                             struct ast_node *id,
                             bool value,
                             struct ast_node *node)
{
    if (id) {
        assert(id->type == AST_NODE_TYPE_IDENTIFIER);
    }
    assert(cnf->num_nodes > 0);


    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        assert(node->op == OPERATOR_NOT
               || node->op == OPERATOR_OR
               || node->op == OPERATOR_AND);        
        switch(node->op) {

        case OPERATOR_NOT: {
            struct ast_node_stack op1_stack = dpll_unit_propagate_recurse(cnf, id, value,
                                                                          ast_get_operand_node( cnf, node, 1));
            
            assert(! ast_node_stack_is_empty(& op1_stack));
            struct ast_node *child = ast_node_stack_peek_addr(& op1_stack);
            if ( ast_node_is_true_constant(child)) {
                struct ast_node_stack s = ast_node_stack_create();
                ast_node_stack_push( &s, & FALSE_CONSTANT_NODE);
                ast_node_stack_free ( &op1_stack);
                return s;
            } else if( ast_node_is_false_constant(child)) {
                struct ast_node_stack s = ast_node_stack_create();
                ast_node_stack_push( &s, & TRUE_CONSTANT_NODE);
                ast_node_stack_free ( &op1_stack);
                return s;
            } else {
                ast_node_stack_push( & op1_stack, node);
                return op1_stack;
            }
            
        } break;


        case OPERATOR_AND: case OPERATOR_OR: {
            struct ast_node *temp1 = ast_get_operand_node( cnf, node, 1);
            struct ast_node *temp2 = ast_get_operand_node( cnf, node, 2);
            
            struct ast_node_stack op1_stack = dpll_unit_propagate_recurse(cnf, id, value, temp1);
            struct ast_node_stack op2_stack = dpll_unit_propagate_recurse(cnf, id, value, temp2);
            
            assert(! ast_node_stack_is_empty(& op1_stack));
            assert(! ast_node_stack_is_empty(& op2_stack));
            
            struct ast_node *op1 = ast_node_stack_peek_addr(& op1_stack);
            struct ast_node *op2 = ast_node_stack_peek_addr(& op2_stack);

            if ( (op1->type == AST_NODE_TYPE_CONSTANT && op2->type == AST_NODE_TYPE_CONSTANT)
                 || (node->op == OPERATOR_AND && (ast_node_is_false_constant(op1) || ast_node_is_false_constant(op2)))
                 || (node->op == OPERATOR_OR && (ast_node_is_true_constant(op1) || ast_node_is_true_constant(op2)))) {
                struct ast_node *pushed;
                const bool and_condition = ast_node_is_true_constant(op1) && ast_node_is_true_constant(op2);
                const bool or_condition = ast_node_is_true_constant(op1) || ast_node_is_true_constant(op2);
                if ( (and_condition && node->op == OPERATOR_AND) || (or_condition && node->op == OPERATOR_OR) ) {
                    pushed = & TRUE_CONSTANT_NODE;
                } else {
                    pushed = & FALSE_CONSTANT_NODE;
                }
                struct ast_node_stack s = ast_node_stack_create();
                ast_node_stack_push( &s, pushed);
                // Discard both subtree's
                // and return constant
                ast_node_stack_free(& op1_stack);
                ast_node_stack_free(& op2_stack);
                return s;
            } else if (op1->type == AST_NODE_TYPE_CONSTANT || op2->type == AST_NODE_TYPE_CONSTANT) {
                assert(!(op1->type == AST_NODE_TYPE_CONSTANT && op2->type == AST_NODE_TYPE_CONSTANT));
                if (op1->type == AST_NODE_TYPE_CONSTANT) {
                    ast_node_stack_free(& op1_stack);
                    return op2_stack;
                } else if (op2->type == AST_NODE_TYPE_CONSTANT) {
                    ast_node_stack_free(& op2_stack);
                    return op1_stack;
                }
            } else {
                // Merging cannot happen further
                dpll_stack_dump_into_stack(& op2_stack, & op1_stack);
                ast_node_stack_free(& op2_stack);
                // Push finally this operator
                ast_node_stack_push(& op1_stack, node);
                return op1_stack;
            }
        } break;

        default:{
            invalid_code_path();
        } break;
        }

            
    } else if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
        if ( id && ast_node_cmp(node, id) ) {
            struct ast_node_stack s = ast_node_stack_create();
            ast_node_stack_push(&s, value ? & TRUE_CONSTANT_NODE : & FALSE_CONSTANT_NODE );
            return s;
        } else {
            struct ast_node_stack s = ast_node_stack_create();
            ast_node_stack_push(&s, node);
            return s;
        }
    } else if ( node->type == AST_NODE_TYPE_CONSTANT) {
        struct ast_node_stack s = ast_node_stack_create();
        ast_node_stack_push(&s, node);
        return s;
        
    } else {
        invalid_code_path();
    }
    // In case where the CNF is empty
    invalid_code_path();
    return (struct ast_node_stack) { 0 };
}



struct ast_node_stack
dpll_unit_propagate( struct ast *cnf,
                     struct ast_node *id,
                     bool value)
{
    return dpll_unit_propagate_recurse(cnf, id, value, ast_end(cnf) - 1 );
}


struct ast_node *
dpll_pick_random_unassigned_literal (struct ast *cnf)
{
    struct symtable *symtable = & global_interpreter.symtable;

    struct ast_node *picked = NULL;

    // we'll pick the first one that we find
    for (struct ast_node *node = ast_begin(cnf);
         node < ast_end(cnf);
         node++) {
        if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
            picked = node;
            break;
        }
    }
          
#if __DEBUG
    if ( picked ) {
        struct symbol_info *syminfo = symtable_syminfo_from_node(symtable, picked);
        assert(!(syminfo->has_value_assigned));
    }
#endif
    return picked;

}

void
dpll_print_solution(bool result)
{
    struct symtable *symtable = & global_interpreter.symtable;
    interpreter_log("DPLL: Found solution ");
    interpreter_log( result ? "`TRUE`" : "`FALSE`");
    interpreter_log(" for input with assigned values \n\t");


    int it = 0;
    char *k;
    void *v;
    ast_symtable_for(it, symtable, k, v) {
        struct symbol_info *syminfo = (struct symbol_info*) v;
        interpreter_log(" [\"%s\"]{ a: %d, v: %d }, ",  k, syminfo->has_value_assigned, syminfo->value );
        log_tabulator();
    }
    interpreter_log( "\n");
}

// @NOTE: The same `cnf` get's reused across recursive calls
bool
dpll_solve_recurse(struct ast *cnf)
{
    bool result = false;
    struct symtable *symtable = & global_interpreter.symtable;
    struct vm_inputs *vmi = & global_interpreter.vmi;

    // DEF: A formula is consistent if it's true.
    //      A formula which is NOT consistent it is NOT necessary false.
    /*    if Φ is a consistent set of literals */
    /*        then return true; */
    if ( dpll_is_consistent(cnf)) {
        dpll_print_solution(true);
        return true;
    }

    //    AST Has been reduced to 0 nodes
    /*    if Φ contains an empty clause */
    /*        then return false; */
    if (dpll_is_empty_or_false_clause(cnf)) {
        dpll_print_solution(false);
        return false;
    }


    struct ast_node *unit_clause = NULL;
    bool is_negated = false;
    while ((unit_clause = dpll_next_unit_clause( cnf, &is_negated ))) {
        printf("\n# Got unit clause: `%.*s`\n", unit_clause->text_len, unit_clause->text);
        bool value = ! is_negated;
        dpll_test_invariant_no_value_assigned(unit_clause);
        // Assign value to it and generate a new ast and propagate
        dpll_identifier_assign_value(unit_clause, value);

        struct ast_node_stack stack = dpll_unit_propagate(cnf, unit_clause, value);
        // @NOTE: `cnf` get's cleared inside `ast_node_stack_dump_xxx`
        ast_node_stack_dump_reversed( &stack, cnf);
        ast_node_stack_free(&stack);
        printf("\n### Getting the propageted result ##### \n");
        ast_dbglog(cnf);
        printf("\n");
    
    }


    // A Pure literal is any literal that does not appear with its' negation in the formula
    // WIKIPEDIA DEF: In the context of a formula in the
    //                conjunctive normal form, a literal is pure if the literal's
    //                complement does not appear in the formula.
    /*    for every literal l that occurs pure in Φ */
    /*       Φ ← pure-literal-assign(l, Φ); */
    { }                         /* Not used in this DPLL implementation */
    // .....

    
    
    struct ast_node *random = dpll_pick_random_unassigned_literal(cnf);
    if ( random ) {
        bool result = false;
        bool assigned_value = true;
        dpll_identifier_assign_value(random, assigned_value);
        struct ast_node_stack s1 = dpll_unit_propagate(cnf, random, assigned_value);
        ast_node_stack_dump_reversed( & s1, cnf);
        ast_node_stack_free(& s1);

        struct ast cnf1 = ast_dup(cnf);
        result |= dpll_solve_recurse(& cnf1);
        ast_free(& cnf1);
        
        if ( !result ) {
            // Continue evaluation
            assigned_value = false;
            dpll_identifier_assign_value(random, assigned_value);
            struct ast_node_stack s2 = dpll_unit_propagate(cnf, random, assigned_value);
            ast_node_stack_dump_reversed( & s2, cnf);
            ast_node_stack_free(& s2);

            struct ast cnf2 = ast_dup(cnf);
            result |= dpll_solve_recurse(& cnf2);
            ast_free(& cnf2);
        
        }
        return result;
        
    } else {
        // No assignment -> last recursive call to determine the result
        return dpll_solve_recurse(cnf);
    }

    
    
    
    
    return result;
}



void
dpll_solve(struct ast *raw_ast)
{
    dpll_preprocess(raw_ast);
    struct ast cnf = dpll_convert_cnf( raw_ast );

    struct ast_node_stack stack = dpll_unit_propagate(& cnf, NULL, 0);
    ast_node_stack_dump_reversed( &stack, & cnf);
    ast_node_stack_free(&stack);

    printf("\n### Getting the preprocess propagated result ##### \n");
    ast_dbglog(& cnf);
    printf("\n");
        

    dpll_solve_recurse(& cnf);

    ast_free(& cnf);
}



#endif /* DPLL_C_IMPL */
