#ifndef AST_INDEXER_C_INCLUDE
#define AST_INDEXER_C_INCLUDE


//#######################################################
#endif /* AST_INDEXER_C_INCLUDE */
#if !defined AST_INDEXER_C_IMPLEMENTED && defined AST_INDEXER_C_IMPL
#define AST_INDEXER_C_IMPLEMENTED
//#######################################################


struct ast_node_child_parent_pair  {
    uint32_t child_uid;
    uint32_t parent_uid;
};



struct ast_indexer {
    
};


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



struct ast_node *
ast_search_parent_node( struct ast *cnf,
                        struct ast_node *child)
{
    return ast_search_parent_node__slow(cnf, child);
}


#include "ast-node-parent-child-pair-stack.h"
#include "uint32-stack.h"

void
ast__assert_monotonicity( struct ast *ast)
{
#if __DEBUG
    uint32_t monotonicity = 0;
    for( struct ast_node *node = ast_begin(ast);
            node < ast_end(ast);
            node ++ ) {
        assert(node->uid != 0);
        assert(node->uid > monotonicity);
        monotonicity = node->uid;
    }
#endif
}



void
ast_node_parent_child_pair_stack__assert_monotonicity( struct ast_node_parent_child_pair_stack *pair_stack)
{
#if __DEBUG
    uint32_t monotonicity = 0;
    for ( struct ast_node_child_parent_pair *pair = ast_node_parent_child_pair_stack_begin(pair_stack);
          pair < ast_node_parent_child_pair_stack_end(pair_stack);
          pair ++) {
        assert(pair->parent_uid != 0);
        assert(pair->child_uid != 0);

        assert(pair->parent_uid > monotonicity);
        monotonicity = pair->parent_uid;
    }

# warning IMPLEMENT ME
#endif
}


uint32_t
ast_node_parent_child_pair_stack_find_child_from_parent(struct ast_node_parent_child_pair_stack *pair_stack,
                                  uint32_t parent_uid)
{
#if __DEBUG
    ast_node_parent_child_pair_stack__assert_monotonicity(pair_stack);
#endif
    for ( struct ast_node_child_parent_pair *pair = ast_node_parent_child_pair_stack_begin(pair_stack);
          pair < ast_node_parent_child_pair_stack_end(pair_stack);
          pair ++) {

        assert(pair->parent_uid != 0);
        assert(pair->child_uid != 0);

        if ( pair->parent_uid == parent_uid ) {
            return pair->child_uid;
        }
    }
    // Not found
    return 0;
}


uint32_t
ast_node_parent_child_pair_stack_find_parent_from_child(struct ast_node_parent_child_pair_stack *pair_stack,
                                  uint32_t child_uid)
{
    for ( struct ast_node_child_parent_pair *pair = ast_node_parent_child_pair_stack_begin(pair_stack);
          pair < ast_node_parent_child_pair_stack_end(pair_stack);
          pair ++) {

        assert(pair->parent_uid != 0);
        assert(pair->child_uid != 0);

        if ( pair->child_uid == child_uid ) {
            return pair->parent_uid;
        }
    }
    // Not found
    return 0;
}






void
ast_node_parent_child_pair_stack_test_invariant_after_creation( struct ast_node_parent_child_pair_stack *pair_stack,
                                          struct ast *ast)
{
    ast__assert_monotonicity(ast);
    ast_node_parent_child_pair_stack__assert_monotonicity(pair_stack);
    
# if __DEBUG
    for( struct ast_node *node = ast_begin(ast);
         node < ast_end(ast);
         node ++ ) {
        
        if (node != (ast_end(ast) - 1)) {
            // Node should be in the stack
#warning @TODO: Node should be in the stack
        }
        
    }
#endif
}
 


static struct ast_indexer
ast_indexer_create_from_node_parent_child_pair_stack(struct ast_node_parent_child_pair_stack *pair_stack)
{
}


struct ast_indexer
ast_indexer_create_from_ast(struct ast *ast)
{
#if __DEBUG
    assert(! is_malformed_formula(ast));
#endif

    assert(!ast_is_empty(ast));

    
    struct ast_node_parent_child_pair_stack pair_stack = {};
    
    uint32_t uid = 1;
    struct ast_node_stack stack = ast_node_stack_create();
    
    // @NOTE: root has no parent, skip it ---> [-2]
    for( struct ast_node *node = ast_begin(ast);
         node < ast_end(ast);
         node ++ ) {

        //assert(!ast_node_stack_is_empty(&stack));
        node->uid = uid++;        
        
        if (node->type == AST_NODE_TYPE_OPERATOR ) {
            for ( uint32_t op = 0; op < operator_num_operands(node); op++) {
                struct ast_node *child = ast_node_stack_peek_addr(&stack);
                struct ast_node_child_parent_pair pair;
                pair.parent_uid = node->uid;
                pair.child_uid = child->uid;
                ast_node_parent_child_pair_stack_push(& pair_stack, &pair);
                ast_node_stack_pop_discard(&stack);
            }
            ast_node_stack_push(&stack, node);
        } else if (node->type == AST_NODE_TYPE_CONSTANT
                   || node->type == AST_NODE_TYPE_IDENTIFIER ) {
            ast_node_stack_push(&stack, node);
        } else {
            invalid_code_path();
        }
    }

    assert(ast_node_stack_num_nodes(& stack) == 1);
    
    assert(ast->num_nodes - 1 == pair_stack.num_pairs);

#if __DEBUG
    ast_node_parent_child_pair_stack_test_invariant_after_creation( &pair_stack, ast);
#endif

    struct ast_indexer result = ast_indexer_create_from_node_parent_child_pair_stack(& pair_stack);
    ast_node_stack_free(& stack);
    ast_node_parent_child_pair_stack_free(& pair_stack);
    return result;
}




#endif /* AST_INDEXER_C_IMPL */
