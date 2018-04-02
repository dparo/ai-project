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

typedef uint64_t ast_truth_value_packed;

struct ast_truth_table_packed {
    // Flexible array member
    size_t num_bits;
    ast_truth_value_packed bits[];
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



struct symbol_table
symbol_table_new(void)
{
    struct symbol_table result = { 0 };
    result.dict = stb_sdict_new(1);
    assert(result.dict);
    return result;
}


size_t
symbol_table_num_ids ( struct symbol_table *symtable )
{
    int result = stb_sdict_count(symtable->dict);
    assert(result >= 0);
    return (size_t) result;
}

void
symbol_table_add_identifier(struct symbol_table *symtable,
                            Token *t)
{
    assert(symtable && symtable->dict);
    assert(t->text);
    assert(t->text_len);

    // null terminate before adding
    char temp = t->text[t->text_len];
    t->text[t->text_len] = 0;
    int result = stb_sdict_set(symtable->dict, t->text, NULL);

    // restore null termination
    t->text[t->text_len] = temp;

}


size_t
symbol_table_get_identifier_value( struct symbol_table *symtable,
                                   Token *t )
{
    assert(symtable && symtable->dict);
    assert(t->text);
    assert(t->text_len);

    char temp = t->text[t->text_len];
    t->text[t->text_len] = 0;
    size_t result = (size_t) stb_sdict_get(symtable->dict, t->text);
    t->text[t->text_len] = temp;
    return result;
}


size_t
ast_truth_table_packed_compute_required_size( size_t required_num_of_bits )
{
    assert(required_num_of_bits);
    return (required_num_of_bits - 1) /
        (sizeof(ast_truth_value_packed) * 8) * sizeof(ast_truth_value_packed) + sizeof(ast_truth_value_packed);
}


struct ast_truth_table_packed*
ast_truth_table_packed_alloc_from_symtable(struct symbol_table *symtable)
{
    struct ast_truth_table_packed* result;

    size_t allocationsize =
        ast_truth_table_packed_compute_required_size(symbol_table_num_ids(symtable));
    assert(allocationsize);

    result = calloc(allocationsize, 1);
    assert(result);

    result->num_bits = symbol_table_num_ids(symtable);

    return result;
}


void
ast_truth_table_pack_bool( struct ast_truth_table_packed *ast_ttp,
                           bool value,
                           size_t bit_index )
{
    assert(bit_index < ast_ttp->num_bits );
    size_t index = bit_index / ( sizeof(ast_truth_value_packed) * 8);
    size_t mask = ast_ttp->bits[index] & ~((size_t)1 << ((size_t)bit_index - index * sizeof(ast_truth_value_packed) * 8));
    ast_ttp->bits[index] = mask | (size_t)value << ((size_t)bit_index - index * sizeof(ast_truth_value_packed) * 8);
#if 0
    printf("bi: %zu | index: %zx | mask: %zx | ast_ttp->bits[index]: %zx\n",
           bit_index, index, mask, ast_ttp->bits[index]);
#endif
}


bool
ast_truth_table_unpack_bool( struct ast_truth_table_packed *ast_ttp,
                             size_t bit_index)
{
    assert(bit_index < ast_ttp->num_bits );
    size_t index = bit_index / ( sizeof(ast_truth_value_packed) * 8);
    size_t local_bit_index = ((size_t)bit_index - index * sizeof(ast_truth_value_packed) * 8);
    size_t mask = ( (size_t) 1 << local_bit_index);
    bool result = (ast_ttp->bits[index] &  mask) >> local_bit_index;
#if 0
        printf("bi: %zu | index: %zx | local_bit_index: %zd | mask: %zx | result: %d\n",
               bit_index, index, local_bit_index, mask, result);
#endif

    return false;
}

void
ast_truth_table_packed_generate_next_combination(struct ast_truth_table_packed *ast_ttp )
{
    assert(ast_ttp->num_bits > 0);

    ast_truth_value_packed *array= ast_ttp->bits;
    size_t bits_count = ast_ttp->num_bits;
    
    size_t nelems;
    if ( bits_count == 0 ) { nelems = 0; }
    else { nelems = ((bits_count - 1) / (sizeof(ast_truth_value_packed) * 8)) + 1; }
   
    size_t bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(ast_truth_value_packed) * 8); }

    for ( ast_truth_value_packed *s = array; s < array + nelems; s ++) {
        // Needs to check for bubble up
        if ( s == (array + nelems - 1)) {
            ast_truth_value_packed checkvalue = ((ast_truth_value_packed) 0 - 1) >> ( sizeof(ast_truth_value_packed) * 8 - bits_count_remainder);
                
            if ( *s == checkvalue ) {
                // Overflow simulation
                memset(array, 0, sizeof(array[nelems]));
                break;
            }
        }
        (*s)++;
        if ( s == 0 ) {
            // Bubble up the add
            continue;
        }
        break;
    }
}

static inline size_t
ast_truth_table_numelems(struct ast_truth_table_packed *ast_ttp)
{
    assert(ast_ttp->num_bits);
    return ( ast_ttp->num_bits - 1 ) / ( sizeof(ast_truth_value_packed) * 8)
        + 1;
}

void
ast_truth_table_packed_dbglog(struct ast_truth_table_packed *ast_ttp)
{
    size_t nelems = ast_truth_table_numelems(ast_ttp);
    for ( size_t i = 0; i < nelems; i ++ ) {
        printf(" %zx |", ast_ttp->bits[i]);
    }
    printf("\n");
}


#endif /* PCALC_UTILS_C_IMPL */

