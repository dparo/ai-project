#ifndef PCALC_UTILS_C_INCLUDE
#define PCALC_UTILS_C_INCLUDE



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-value"
#define STB_DEFINE
#include "stb.h"
#pragma GCC diagnostic pop



struct symbol_table {
    stb_sdict *dict;
};

struct ast_token_stack {
#define AST_TOKEN_STACK_MAX_TOKENS_COUNT 1024
    Token tokens[AST_TOKEN_STACK_MAX_TOKENS_COUNT];
    size_t num_tokens;
};
    

    

struct ast_token_queue {
#define AST_TOKEN_QUEUE_MAX_TOKENS_COUNT 1024
    Token tokens[AST_TOKEN_QUEUE_MAX_TOKENS_COUNT];
    size_t num_tokens;

};

//#######################################################
#endif /* PCALC_UTILS_C_INCLUDE */
#if !defined PCALC_UTILS_C_IMPLEMENTED && defined PCALC_UTILS_C_IMPL
#define PCALC_UTILS_C_IMPLEMENTED
//#######################################################



void
ast_token_stack_push(struct ast_token_stack *stack,
                     Token *token)
{
    assert(stack->num_tokens != AST_TOKEN_STACK_MAX_TOKENS_COUNT);
    stack->tokens[(stack->num_tokens) ++] = *token;
}


Token*
ast_token_stack_peek_addr(struct ast_token_stack *stack)
{
    assert(stack->num_tokens);
    return &(stack->tokens[stack->num_tokens - 1]);
}

void
ast_token_stack_pop(struct ast_token_stack *stack)
{
    assert(stack->num_tokens);
    (stack->num_tokens) --;
}

Token
ast_token_stack_pop_value(struct ast_token_stack *stack)
{
    assert(stack->num_tokens);
    Token result = stack->tokens[--(stack->num_tokens)];
    return result;
}



// Highly discouraged pointer may point to invalid
// memory after a new push
Token*
ast_token_stack_pop_value_addr(struct ast_token_stack *stack)
{
    assert(stack->num_tokens);
    Token *result = &(stack->tokens[--(stack->num_tokens)]);
    return result;
}




void
ast_token_queue_push(struct ast_token_queue *queue,
                     Token *token)
{
    assert(queue->num_tokens != AST_TOKEN_QUEUE_MAX_TOKENS_COUNT);
    queue->tokens[(queue->num_tokens) ++] = *token;
}





#endif /* PCALC_UTILS_C_IMPL */

