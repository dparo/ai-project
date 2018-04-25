#ifndef DPLL_UNIT_PROPAGATE_OLD_C_INCLUDE
#define DPLL_UNIT_PROPAGATE_OLD_C_INCLUDE


//#######################################################
#endif /* DPLL_UNIT_PROPAGATE_OLD_C_INCLUDE */
#if !defined DPLL_UNIT_PROPAGATE_OLD_C_IMPLEMENTED && defined DPLL_UNIT_PROPAGATE_OLD_C_IMPL
#define DPLL_UNIT_PROPAGATE_OLD_C_IMPLEMENTED
//#######################################################


struct unitprop_res {
    struct ast_node *reduced_node;
};


struct unitprop_res 
dpll_unit_propagate_recurse( struct ast *cnf,
                             struct ast_node *id,
                             bool value,
                             struct ast_node_stack *result,
                             struct ast_node *node)
{
    assert(id);
    assert(id->type == AST_NODE_TYPE_IDENTIFIER);
    assert(cnf->num_nodes > 0);


    if ( node->type == AST_NODE_TYPE_OPERATOR ) {
        assert(node->op == OPERATOR_NOT
               || node->op == OPERATOR_OR
               || node->op == OPERATOR_AND);        
        switch(node->op) {

        case OPERATOR_NOT: {
            struct ast_node *child_node = ast_get_operand_node( cnf, node, 1);

            struct unitprop_res merged =
                dpll_unit_propagate_recurse(cnf, child_node, result, id, value );
            if ( ast_node_is_true_constant(merged.reduced_node)) {
                return (unitprop_res) { & FALSE_CONSTANT_NODE };
            } else if ( ast_node_is_false_constant(merged.reduced_node)) {
                return (unitprop_res) { & TRUE_CONSTANT_NODE };
            } else {
                ast_node_stack_push(result, (child_value  
                                             ? & FALSE_CONSTANT_NODE
                                             : & TRUE_CONSTANT_NODE ));
                ast_node_stack_push(result, node);
                
                // return this `NOT` operator
                return (unitprop_res) { node };
            }
        } break;

        case OPERATOR_AND: case OPERATOR_OR: {
            struct ast_node *op2 = ast_get_operand_node( cnf, node, 2);
            struct ast_node *op1 = ast_get_operand_node( cnf, node, 1);

            bool merged = false;
            bool child_value = false;
            dpll_unit_propagate_recurse(cnf, op2, result, id, value, &merged, & child_value);
            if ( merged ) {
                if ( node->op == OPERATOR_AND && child_value == 0 ) {
                    ast_node_stack_push(result, & FALSE_CONSTANT_NODE);
                    *was_reduced = true;
                    *reduced_value = false;
                } else if (node->op == OPERATOR_OR && child_value == 1) {
                    ast_node_stack_push(result, & TRUE_CONSTANT_NODE);
                    *was_reduced = true;
                    *reduced_value = true;
                } else {
                    *was_reduced = false;
                    ast_node_stack_push(result, op1);
                }
                break;
            }

            dpll_unit_propagate_recurse(cnf, op1, result, id, value, &merged, & child_value);
            if ( merged ) {
                if ( node->op == OPERATOR_AND && child_value == 0 ) {
                    ast_node_stack_push(result, & FALSE_CONSTANT_NODE);
                    *was_reduced = true;
                    *reduced_value = false;
                } else if (node->op == OPERATOR_OR && child_value == 1) {
                    ast_node_stack_push(result, & TRUE_CONSTANT_NODE);
                    *was_reduced = true;
                    *reduced_value = true;
                } else {
                    *was_reduced = false;
                    ast_node_stack_push(result, op2);
                }
                break;
            }

        } break;

        default:{
            invalid_code_path();
        } break;
        }

            
    } else if ( node->type == AST_NODE_TYPE_IDENTIFIER ) {
        if ( ast_node_cmp(node, id) ) {
            // ast_node_stack_push(result, & TRUE_CONSTANT_NODE);
            *was_reduced = true;
            *reduced_value = true;
        } else {
            ast_node_stack_push(result, node);
            *was_reduced = false;
        }
    } else if ( node->type == AST_NODE_TYPE_CONSTANT) {
        // Constant can only should only found as top level
        assert(node == ast_end(cnf) - 1);
        // ast_node_stack_push(result, node);
        *was_reduced = false;
        *reduced_value = ast_node_is_true_constant(node) ? true : false;
    } else {
        invalid_code_path();
    }
    
}








#endif /* DPLL_UNIT_PROPAGATE_OLD_C_IMPL */
