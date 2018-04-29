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
#ifndef AST_INDEXING_C_INCLUDE
#define AST_INDEXING_C_INCLUDE


//#######################################################
#endif /* AST_INDEXING_C_INCLUDE */
#if !defined AST_INDEXING_C_IMPLEMENTED && defined AST_INDEXING_C_IMPL
#define AST_INDEXING_C_IMPLEMENTED
//#######################################################


static bool
is_malformed_formula(struct ast* ast)
{
    uint32_t it = 1;
    struct ast_node *node = ast_end(ast) - 1;
    for ( ;
          node >= ast_begin(ast);
          node --, it--) {
        if (it == 0) {
            break;
        }
        if (ast_node_is_operator(node)) {
            it += operator_num_operands(node);
        }
    }
    
    if ( (node != ast->nodes - 1) || it != 0) {
        return true;
    }
    return false;
}



// Fixes order of the operands on the queue, A & B, in the queue
// becomes { [0] = A, [2] = B, [3] = &} Which means that to know
// the position of the first operand we need to process recursively
// the second operand
struct ast_node *
ast_get_operand_node__slow ( struct ast *ast,
                             struct ast_node *op_node,
                             uint32_t operand_num )
{
    assert(op_node >= ast->nodes);
    assert(op_node < ast_end(ast));
    
    assert( ast_node_is_operator(op_node));
    uint32_t it = 1;
    struct ast_node *node = op_node;
    for ( ;
         node >= ast_begin(ast);
         node --, it--) {
        if ( operand_num == (it) &&
             node != op_node) {
            break;
        }
        if (ast_node_is_operator(node)) {
            it += operator_num_operands(node);
        }
    }

#if __DEBUG
    assert(! is_malformed_formula(ast));
#endif
    return node;
}

struct ast_node *
ast_get_operand_node ( struct ast *ast,
                       struct ast_node *op_node,
                       uint32_t operand_num )
{
    return ast_get_operand_node__slow(ast, op_node, operand_num);
}




// Slow n^2 algorithm
struct ast_node *
ast_search_parent_node__slow( struct ast *cnf,
                              struct ast_node *child)
{
    assert(child >= ast_begin(cnf));
    struct ast_node *result = NULL;

    for( struct ast_node *node = ast_end(cnf) - 1;
         node > child;
         node -- ) {
        uint32_t numofoperands = operator_num_operands(node);
        for( uint32_t operand_num = 1;
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




void
ast__assert_monotonicity( struct ast *ast)
{
#if __DEBUG
    assert( ! ast_is_empty(ast));
    uint32_t monotonicity = (ast_begin(ast)) -> uid;
    assert(monotonicity == 1);
    for( struct ast_node *node = ast_begin(ast) + 1;
            node < ast_end(ast);
            node ++ ) {
        assert(node->uid != 0);
        assert(node->uid > monotonicity);
        assert(node->uid == monotonicity + 1);
        monotonicity = node->uid;
    }
#endif
}


// @NOTE: Tested seems to work, monotonicity though does not
// we have breakpoints causes by merges over the deletion of `ANDS` and `ORS`
// operator which causes discontinueties. The only way to avoid discontinuities
// is using the uid to index in the original AST.
// Or much simply we fix those discontinueties this will lead to more difficult
//   indexing of the child nodes.
struct ast_node *
ast_node_from_uid( struct ast* ast,
                   uint32_t uid)
{
#if __DEBUG
    ast__assert_monotonicity(ast);
#endif
    assert(! ast_is_empty(ast));


    struct ast_node *result;
    if (uid != 0 ) {
        size_t index = (uid - 1);
        result = &(ast->nodes[index]);
    } else {
        result = 0;
    }


    
    assert (result->uid != 0);
    assert (ast_node_is_valid(result));
    
    return result;
}


struct ast_node *
ast_search_parent_node( struct ast *ast,
                        struct ast_node *child)
{
#if __DEBUG
    ast__assert_monotonicity(ast);
    struct ast_node *check = ast_search_parent_node__slow(ast, child);

    if (check == NULL) {
        int breakme = 0;
    }
#endif
    
    struct ast_node *result;
    if (child->parent != 0) {
        result = ast_node_from_uid(ast, child->parent);
    }
    else {
        result = NULL;
    }


#if __DEBUG    
    if (check == NULL ) {
        assert(result == NULL);
    } else {
        assert(memcmp(result, check, sizeof(*result)) == 0);
    }
#endif

    return result;
}




#include "ast_node_stack_ref.h"


void
ast_rebuild_uids(struct ast *ast)
{
#if __DEBUG
    assert(! is_malformed_formula(ast));
#endif

    assert(!ast_is_empty(ast));

   
    (ast_end(ast) - 1)->parent = 0;    

    size_t num_nodes = ast_num_nodes(ast);
    uint32_t uid = 1;
    struct ast_node_ref_stack stack = ast_node_ref_stack_create();
    
    // @NOTE: root has no parent, skip it ---> [-2]
    for( struct ast_node *node = ast_begin(ast);
         node < ast_end(ast);
         node ++ ) {

        //assert(!ast_node_stack_is_empty(&stack));
        node->uid = uid;
        uid ++;
        
        if (node->type == AST_NODE_TYPE_OPERATOR ) {
            for ( uint32_t op = 0; op < operator_num_operands(node); op++) {
                struct ast_node **child = ast_node_ref_stack_peek_addr(&stack);
                (*child)->parent = node->uid;
                ast_node_ref_stack_pop_discard(&stack);
            }
            ast_node_ref_stack_push(&stack, & node);
        } else if (node->type == AST_NODE_TYPE_CONSTANT
                   || node->type == AST_NODE_TYPE_IDENTIFIER ) {
            ast_node_ref_stack_push(&stack, & node);
        } else {
            invalid_code_path();
        }
    }

    (ast_end(ast) - 1)->parent = 0;

    
    assert(ast_node_ref_stack_num_nodes(& stack) == 1);
    
    ast_node_ref_stack_free(& stack);
#if __DEBUG    
    ast__assert_monotonicity(ast);
#endif
}



#endif /* AST_INDEXING_C_IMPL */
