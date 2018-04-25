#ifndef DPLL_CNF_TEST_C_INCLUDE
#define DPLL_CNF_TEST_C_INCLUDE


//#######################################################
#endif /* DPLL_CNF_TEST_C_INCLUDE */
#if !defined DPLL_CNF_TEST_C_IMPLEMENTED && defined DPLL_CNF_TEST_C_IMPL
#define DPLL_CNF_TEST_C_IMPLEMENTED
//#######################################################


#define TEST_CNF_WITH_BRUTEFORCE_EVALUATION 1

#if __DEBUG
bool
eval_ast(struct ast *ast);


void
ast_dump_into(struct ast *dumper,
              struct ast *dumpee)
{
    for(struct ast_node *node = ast_begin(dumper);
        node < ast_end(dumper);
        node ++) {
        ast_push(dumpee, node);
    }
}

void
test_bruteforce_formula_equality( struct ast *raw_ast,
                                  struct ast *generated_ast)
{
#if TEST_CNF_WITH_BRUTEFORCE_EVALUATION
    // Checks equality between the generated_ast and the original raw_ast
    // You should see all `1`s in the last column in every stage
    // of the conversion
    static struct ast_node EQ_EQ_NODE =
        { "==", 2, 2, AST_NODE_TYPE_OPERATOR, OPERATOR_EQUAL_EQUAL, DELIMITER_NONE };

    struct ast temp = ast_dup(generated_ast);
    ast_dump_into( raw_ast, &temp);
    ast_push( & temp, & EQ_EQ_NODE); 
    eval_ast( & temp );


    interpreter_log("\n\n");
    ast_free(&temp);
#endif
}


#endif

void
test_dpll_operator_conversion_invariant(struct ast *raw_ast,
                                        struct ast* generated_ast)
{
#if __DEBUG
    test_bruteforce_formula_equality( raw_ast, generated_ast);
        
    for (struct ast_node *node = ast_end(generated_ast) - 1;
         node >= ast_begin(generated_ast);
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
test_dpll_demorgan_invariant(struct ast* raw_ast,
                             struct ast* generated_ast)
{
    test_bruteforce_formula_equality( raw_ast, generated_ast);
#if __DEBUG
    for (struct ast_node *node = ast_end(generated_ast) - 1;
         node >= ast_begin(generated_ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_NOT ) {
                struct ast_node *op1_node = ast_get_operand_node( generated_ast, node, 1);
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
test_dpll_double_negation_elimination_invariant(struct ast *raw_ast,
                                                struct ast *generated_ast)
{
    test_bruteforce_formula_equality( raw_ast, generated_ast);
#if __DEBUG
    for (struct ast_node *node = ast_end(generated_ast) - 1;
         node >= ast_begin(generated_ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_NOT ) {
                struct ast_node *op1_node = ast_get_operand_node( generated_ast, node, 1);
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
test_dpll_or_distribution_invariant( struct ast* raw_ast,
                                     struct ast *generated_ast)
{
#if __DEBUG
    test_bruteforce_formula_equality( raw_ast, generated_ast);

    test_dpll_demorgan_invariant(raw_ast, generated_ast);
    test_dpll_double_negation_elimination_invariant(raw_ast, generated_ast);

    for (struct ast_node *node = ast_end(generated_ast) - 1;
         node >= ast_begin(generated_ast);
         node -- ) {
        if (node->type == AST_NODE_TYPE_OPERATOR) {
            if (node->op == OPERATOR_OR ) {
                struct ast_node *op1_node = ast_get_operand_node( generated_ast, node, 1);
                struct ast_node *op2_node = ast_get_operand_node( generated_ast, node, 2);
                if ( op1_node->type == AST_NODE_TYPE_OPERATOR)
                    assert(op1_node->op != OPERATOR_AND);
                if ( op2_node->type == AST_NODE_TYPE_OPERATOR)
                    assert(op2_node->op != OPERATOR_AND);
                
            } else if (node->op == OPERATOR_AND ) {
            } else if (node->op == OPERATOR_NOT) {
                struct ast_node *op1_node = ast_get_operand_node( generated_ast, node, 1);
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










#endif /* DPLL_CNF_TEST_C_IMPL */
