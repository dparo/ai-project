#ifndef DPLL_TEST_C_INCLUDE
#define DPLL_TEST_C_INCLUDE


//#######################################################
#endif /* DPLL_TEST_C_INCLUDE */
#if !defined DPLL_TEST_C_IMPLEMENTED && defined DPLL_TEST_C_IMPL
#define DPLL_TEST_C_IMPLEMENTED
//#######################################################
void
test_dpll_operator_conversion_invariant(struct ast* ast)
{
#if __DEBUG
    for (struct ast_node *node = ast_end(ast) - 1;
         node >= ast_begin(ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            assert( node->op == OPERATOR_NOT || node->op == OPERATOR_OR ||
                    node->op == OPERATOR_AND);
        } else if (node->type == AST_NODE_TYPE_IDENTIFIER ||
                   node->type == AST_NODE_TYPE_CONSTANT) {
            // GOOD !
        } else {
            assert(0);
        }
    }
#endif
}

void
test_dpll_demorgan_invariant(struct ast* ast)
{
#if __DEBUG
    for (struct ast_node *node = ast_end(ast) - 1;
         node >= ast_begin(ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_NOT ) {
                struct ast_node *op1_node = ast_get_operand_node( ast, node, 1);
                if (op1_node->type == AST_NODE_TYPE_OPERATOR ) {
                    assert(op1_node->op == OPERATOR_NOT);
                } else if (op1_node->type == AST_NODE_TYPE_IDENTIFIER ||
                           op1_node->type == AST_NODE_TYPE_CONSTANT) {
                    // GOOD..
                } else {
                    assert(0);
                }
            }
        }
    }
#endif
}

void
test_dpll_double_negation_elimination_invariant(struct ast *ast)
{
#if __DEBUG
        for (struct ast_node *node = ast_end(ast) - 1;
         node >= ast_begin(ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_NOT ) {
                struct ast_node *op1_node = ast_get_operand_node( ast, node, 1);
                if (op1_node->type == AST_NODE_TYPE_OPERATOR ) {
                    assert(op1_node->op != OPERATOR_NOT);
                } else if (op1_node->type == AST_NODE_TYPE_IDENTIFIER ||
                           op1_node->type == AST_NODE_TYPE_CONSTANT) {
                    // GOOD..
                } else {
                    assert(0);
                }
            } else {
                // If it's an operator, but it's not the negation OP, make sure it is either and AND
                // or an OR
                assert(node->op == OPERATOR_OR || node->op == OPERATOR_AND);
            }
        }
    }
#endif
}


void
test_dpll_or_distribution_invariant(struct ast *ast)
{
#if __DEBUG
    test_dpll_operator_conversion_invariant(ast);
    test_dpll_demorgan_invariant(ast);
    test_dpll_double_negation_elimination_invariant(ast);

    for (struct ast_node *node = ast_end(ast) - 1;
         node >= ast_begin(ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_OR ) {
                struct ast_node *op1_node = ast_get_operand_node( ast, node, 1);
                struct ast_node *op2_node = ast_get_operand_node( ast, node, 2);
                if ( op1_node->type == AST_NODE_TYPE_OPERATOR)
                    assert(op1_node->op != OPERATOR_AND);
                if ( op2_node->type == AST_NODE_TYPE_OPERATOR)
                    assert(op2_node->op != OPERATOR_AND);
                
            } else if (node->op == OPERATOR_AND ) {
            } else if (node->op == OPERATOR_NOT) {
                struct ast_node *op1_node = ast_get_operand_node( ast, node, 1);
                assert(op1_node->type == AST_NODE_TYPE_IDENTIFIER ||
                       op1_node->type == AST_NODE_TYPE_CONSTANT);
            } else {
                assert(node->type == AST_NODE_TYPE_IDENTIFIER ||
                       node ->type == AST_NODE_TYPE_CONSTANT);
            }
        }
    }
#endif
}








#endif /* DPLL_TEST_C_IMPL */
