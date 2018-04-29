/*
 * Copyright (C) 2018  Davide Paro
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef DPLL_CNF_TEST_C_INCLUDE
#define DPLL_CNF_TEST_C_INCLUDE


//#######################################################
#endif /* DPLL_CNF_TEST_C_INCLUDE */
#if !defined DPLL_CNF_TEST_C_IMPLEMENTED && defined DPLL_CNF_TEST_C_IMPL
#define DPLL_CNF_TEST_C_IMPLEMENTED
//#######################################################


#define TEST_CNF_WITH_BRUTEFORCE_EVALUATION 0

#if __DEBUG


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
    struct ast temp = ast_dup(generated_ast);
    ast_dump_into( raw_ast, &temp);
    ast_push( & temp, & EQ_EQ_NODE); 
    eval_ast( & temp, BRUTEFORCE_SOLVER);

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
#if __DEBUG
    test_bruteforce_formula_equality( raw_ast, generated_ast);
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
#if __DEBUG
    test_bruteforce_formula_equality( raw_ast, generated_ast);
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
