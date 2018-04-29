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


#include "ast-node-child-parent-pair-stack.h"
#include "uint32-stack.h"



// Used to obtain a log(n) search of the childs from the
// given parent (binary search with uids)
struct ast_indexer {
    struct ast_node_child_parent_pair_stack parent_to_child;
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


    uint32_t root_uid = (ast_end(ast) - 1)->uid;

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
    if (child->parent != 0)
        result = ast_node_from_uid(ast, child->parent);
    else
        result = NULL;


#if __DEBUG    
    if (check == NULL ) {
            assert(result == NULL);
    } else {
        assert(memcmp(result, check, sizeof(*result)) == 0);
    }
#endif

    
    return result;
}





void
ast_node_child_parent_pair_stack__assert_parent_monotonicity( struct ast_node_child_parent_pair_stack *pair_stack)
{
#if __DEBUG


    
# if 0
    uint32_t parent_monotonicity = (ast_node_child_parent_pair_stack_begin(pair_stack))->parent_uid;
    for ( struct ast_node_child_parent_pair *pair = ast_node_child_parent_pair_stack_begin(pair_stack) + 1;
          pair < ast_node_child_parent_pair_stack_end(pair_stack);
          pair ++) {

        assert(pair->parent_uid != 0);
        assert(pair->child_uid != 0);
        assert(pair->parent_uid <= parent_monotonicity);
        parent_monotonicity = pair->parent_uid;        
    }
#endif

    

# warning IMPLEMENT ME
#endif
}


uint32_t
ast_indexer_get_child_uid_from_parent__linear(struct ast_indexer *indexer,
                                              uint32_t parent_uid)
{
    struct ast_node_child_parent_pair_stack *parent_to_child = &indexer->parent_to_child;
#if __DEBUG
    ast_node_child_parent_pair_stack__assert_parent_monotonicity(parent_to_child);
#endif
    for ( struct ast_node_child_parent_pair *pair = ast_node_child_parent_pair_stack_begin(parent_to_child);
          pair < ast_node_child_parent_pair_stack_end(parent_to_child);
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


 
void
ast_indexer_test_invariant_after_creation( struct ast_indexer *index,
                                           struct ast *ast)
{
    ast__assert_monotonicity(ast);
    ast_node_child_parent_pair_stack__assert_parent_monotonicity(& index->parent_to_child);
    
# if __DEBUG

    //@NOTE: Don't test to find parent node for the last one
    for( struct ast_node *node = ast_begin(ast);
         node < ast_end(ast) - 1;
         node ++ ) {
        //struct ast_node *parent = &(ast->nodes[node->parent + ast->num_nodes - 1]);
        //assert ( parent == ast_search_parent_node__slow(ast, node));

        // Make sure to find every node
        struct ast_node *parent = ast_search_parent_node(ast, node);
        assert(parent != 0);
    }
#endif
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
                struct ast_node_child_parent_pair pair;
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


struct ast_indexer
ast_indexer_create_from_ast(struct ast *ast)
{
#if __DEBUG
    assert(! is_malformed_formula(ast));
#endif

    assert(!ast_is_empty(ast));

    
    struct ast_node_child_parent_pair_stack pair_stack = {};

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
                struct ast_node_child_parent_pair pair;
                (*child)->parent = node->uid;
                pair.parent_uid = node->uid;
                pair.child_uid = (*child)->uid;
                ast_node_child_parent_pair_stack_push(& pair_stack, &pair);
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
    
    assert(ast->num_nodes - 1 == pair_stack.num_pairs);

    struct ast_indexer result;
    result.parent_to_child = pair_stack;
    ast_node_ref_stack_free(& stack);
#if __DEBUG    
    ast_indexer_test_invariant_after_creation(& result, ast);
#endif
    return result;
}


void
ast_indexer_free(struct ast_indexer *indexer)
{
    ast_node_child_parent_pair_stack_free(& (indexer->parent_to_child));
}




#endif /* AST_INDEXER_C_IMPL */
