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


#define BOOL_PACKED_ARRAY_NELEMS(bitscount, typeof_arraymember)  \
    ((((bitscount) - 1) / (sizeof(typeof_arraymember) * 8)) + 1)

#define BOOL_PACKED_ARRAY_SIZE(bitscount, typeof_arraymember)      \
    BOOL_PACKED_ARRAY_NELEMS(bitscount, typeof_arraymember) * sizeof(typeof_arraymember)

#define BOOL_PACK_INTO_ARRAY(val, bit_index, array, array_num_members, typeof_arraymember) \
    do {                                                                \
        assert(bit_index < (array_num_members) * sizeof(typeof_arraymember) * 8); \
        size_t index = ((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8); \
        (array)[index] =                                                \
            (typeof_arraymember)((array)[index]                         \
                                 & ~((typeof_arraymember)1 << ((size_t)(bit_index) - index * sizeof(typeof_arraymember) * 8))) \
            | (typeof_arraymember)(val) << ((size_t)(bit_index) - index * sizeof(typeof_arraymember) * 8); \
    } while(0)

#define BOOL_UNPACK_FROM_ARRAY(bit_index, array, array_num_members, typeof_arraymember) \
    (assert(bit_index < (array_num_members) * sizeof(typeof_arraymember) * 8), \
     (((array)[((size_t)(((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8)))] \
       & ((typeof_arraymember) 1 << (((size_t)(bit_index) - ((size_t)(((size_t)bit_index) \
                                                                      / ( sizeof(typeof_arraymember) * 8))) * sizeof(typeof_arraymember) * 8)))) \
      >> ((size_t)(bit_index) - ((size_t)(((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8))) * sizeof(typeof_arraymember) * 8))) \


struct symbol_table {
    stb_sdict *dict;
};

struct ast_token_stack {
#define AST_TOKEN_STACK_MAX_TOKENS_COUNT 1024
    Token tokens[AST_TOKEN_STACK_MAX_TOKENS_COUNT];
    size_t num_tokens;
};

typedef uint32_t ast_packed_bool;

struct ast_computation_stack {
#define AST_COMPUTATION_STACK_MAX_NUMBITS 1024
#define AST_COMPUTATION_STACK_MAX_NELEMS \
    BOOL_PACKED_ARRAY_NELEMS(AST_COMPUTATION_STACK_MAX_NUMBITS, sizeof(ast_packed_bool))

    ast_packed_bool bits[AST_COMPUTATION_STACK_MAX_NELEMS];
    bool bools[AST_COMPUTATION_STACK_MAX_NUMBITS];
    size_t num_bits;
};



struct ast_truth_table_packed {
#define AST_TRUTH_TABLE_MAX_NUMBITS 1024
#define AST_TRUTH_TABLE_MAX_NUMELEMS \
    BOOL_PACKED_ARRAY_NELEMS(AST_TRUTH_TABLE_MAX_NUMBITS, sizeof(ast_packed_bool))
    // Flexible array member
    ast_packed_bool bits[AST_TRUTH_TABLE_MAX_NUMELEMS];
    size_t num_bits;
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


static inline size_t
ast_truth_table_numelems(struct ast_truth_table_packed *ast_ttp)
{
    assert(ast_ttp->num_bits);
    return ( ast_ttp->num_bits - 1 ) / ( sizeof(ast_packed_bool) * 8)
        + 1;
}
static inline size_t
ast_truth_table_size(struct ast_truth_table_packed *ast_ttp)
{
    assert(ast_ttp->num_bits);
    return ast_truth_table_numelems(ast_ttp) * sizeof(ast_packed_bool);
}


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
ast_computation_stack_push(struct ast_computation_stack *stack,
                           bool v)
{
    assert(stack->num_bits != AST_COMPUTATION_STACK_MAX_NUMBITS);
    stack->bools[(stack->num_bits) ++] = v;
}


bool
ast_computation_stack_peek_value(struct ast_computation_stack *stack)
{
    assert(stack->num_bits);
    return stack->bools[stack->num_bits - 1];
}

void
ast_computation_stack_pop(struct ast_computation_stack *stack)
{
    assert(stack->num_bits);
    (stack->num_bits) --;
}

bool
ast_computation_stack_pop_value(struct ast_computation_stack *stack)
{
    assert(stack->num_bits);
    bool result = stack->bools[--(stack->num_bits)];
    return result;
}



// Highly discouraged pointer may point to invalid
// memory after a new push
bool*
ast_computation_stack_pop_value_addr(struct ast_computation_stack *stack)
{
    assert(stack->num_bits);
    bool *result = &(stack->bools[--(stack->num_bits)]);
    return result;
}



#define ast_token_queue_for( iterator, queue, token)        \
    for (((iterator) = 0), ((token) = (queue).tokens);      \
         ((iterator) < (queue).num_tokens);             \
         ((token) = & ((queue).tokens[++(iterator)])))      \
    if (true)


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


void
ast_truth_table_packed_alloc_from_symtable(struct ast_truth_table_packed *ast_ttp,
                                           struct symbol_table *symtable)
{
    assert(symbol_table_num_ids(symtable) < AST_TRUTH_TABLE_MAX_NUMBITS);
    ast_ttp->num_bits = symbol_table_num_ids(symtable);
    // assert_msg(0, "Wrong size for memset");
    memset(ast_ttp->bits, 0, ast_truth_table_size(ast_ttp));
}


void
ast_truth_table_pack_bool( struct ast_truth_table_packed *ast_ttp,
                           bool value,
                           size_t bit_index )
{
    assert(bit_index < ast_ttp->num_bits );
# if __DEBUG
    size_t index = bit_index / ( sizeof(ast_packed_bool) * 8);
    size_t mask = ast_ttp->bits[index] & ~((size_t)1 << ((size_t)bit_index - index * sizeof(ast_packed_bool) * 8));
    ast_ttp->bits[index] = mask | (size_t)value << ((size_t)bit_index - index * sizeof(ast_packed_bool) * 8);

    { // TODO: The above code has moved in the BOOL_PACK_IN_ARRAY define
        // should check that both the ways to do it are identical
        ast_packed_bool temp = ast_ttp->bits[index];
#endif
        BOOL_PACK_INTO_ARRAY( value, bit_index, ast_ttp->bits,
                            ast_truth_table_numelems(ast_ttp),
                            ast_packed_bool );
#if __DEBUG
        assert ( temp == (bit_index / ( sizeof(ast_packed_bool) * 8)));
    }
#endif
    
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
    bool result;
#if __DEBUG
    size_t index = bit_index / ( sizeof(ast_packed_bool) * 8);
    size_t local_bit_index = ((size_t)bit_index - index * sizeof(ast_packed_bool) * 8);
    size_t mask = ( (size_t) 1 << local_bit_index);
    result = (ast_ttp->bits[index] &  mask) >> local_bit_index;

#endif
    bool temp = BOOL_UNPACK_FROM_ARRAY(bit_index, ast_ttp->bits,
                                       ast_truth_table_numelems(ast_ttp),
                                       ast_packed_bool);
#if __DEBUG
    assert(result == temp);
#endif
    result = temp;

    
#if 0
        printf("bi: %zu | index: %zx | local_bit_index: %zd | mask: %zx | result: %d\n",
               bit_index, index, local_bit_index, mask, result);
#endif

    return result;
}

void
ast_truth_table_packed_generate_next_combination(struct ast_truth_table_packed *ast_ttp )
{
    assert(ast_ttp->num_bits > 0);

    ast_packed_bool *array= ast_ttp->bits;
    size_t bits_count = ast_ttp->num_bits;
    
    size_t nelems;
    if ( bits_count == 0 ) { nelems = 0; }
    else { nelems = ((bits_count - 1) / (sizeof(ast_packed_bool) * 8)) + 1; }
   
    size_t bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(ast_packed_bool) * 8); }

    for ( ast_packed_bool *s = array; s < array + nelems; s ++) {
        // Needs to check for bubble up
        if ( s == (array + nelems - 1)) {
            ast_packed_bool checkvalue = ((ast_packed_bool) 0 - 1) >> ( sizeof(ast_packed_bool) * 8 - bits_count_remainder);
                
            if ( *s == checkvalue ) {
                // Overflow simulation
                assert(sizeof(array[nelems]) == ast_truth_table_size(ast_ttp));
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

void
ast_truth_table_packed_dbglog(struct ast_truth_table_packed *ast_ttp)
{
    size_t nelems = ast_truth_table_numelems(ast_ttp);
    for ( size_t i = 0; i < nelems; i ++ ) {
        printf(" %zx |", (size_t)ast_ttp->bits[i]);
    }
    printf("\n");
}


void
ast_computation_stack_dbglog(struct ast_computation_stack *stack)
{
    size_t nelems = stack->num_bits;
    printf("AST_COMPUTATION_STACK_DBG_LOG: ");
    for ( size_t i = 0; i < nelems; i ++ ) {
        printf(" %d |", stack->bools[i]);        
    }
    printf("\n");
}


#endif /* PCALC_UTILS_C_IMPL */

