#ifndef INTERPRETER_UTILS_C_INCLUDE
#define INTERPRETER_UTILS_C_INCLUDE



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


struct symtable {
    stb_sdict *dict;
};

struct token_stack {
#define TOKEN_STACK_MAX_TOKENS_COUNT 1024
    Token tokens[TOKEN_STACK_MAX_TOKENS_COUNT];
    size_t num_tokens;
};

typedef uint32_t packed_bool;

struct vm_stack {
#define VM_STACK_MAX_NUMBITS 1024
#define VM_STACK_MAX_NELEMS \
    BOOL_PACKED_ARRAY_NELEMS(VM_STACK_MAX_NUMBITS, sizeof(packed_bool))

    packed_bool bits[VM_STACK_MAX_NELEMS];
    size_t num_bits;
};



struct vm_inputs {
#define VM_INPUTS_MAX_NUMBITS 1024
#define VM_INPUTS_MAX_NUMELEMS \
    BOOL_PACKED_ARRAY_NELEMS(VM_INPUTS_MAX_NUMBITS, sizeof(packed_bool))
    // Flexible array member
    packed_bool inputs[VM_INPUTS_MAX_NUMELEMS];
    size_t num_inputs;
};


struct vm_outputs {
#define VM_OUTPUTS_MAX_NUMBITS 1024
#define VM_OUTPUTS_MAX_NUMELEMS \
    BOOL_PACKED_ARRAY_NELEMS(VM_OUTPUTS_MAX_NUMBITS, sizeof(packed_bool))
    // Flexible array member
    packed_bool outputs[VM_OUTPUTS_MAX_NUMELEMS];
    size_t num_outputs;
};

    

struct ast {
#define AST_MAX_TOKENS_COUNT 1024
    Token tokens[AST_MAX_TOKENS_COUNT];
    size_t num_tokens;
};



//#######################################################
#endif /* INTERPRETER_UTILS_C_INCLUDE */
#if !defined INTERPRETER_UTILS_C_IMPLEMENTED && defined INTERPRETER_UTILS_C_IMPL
#define INTERPRETER_UTILS_C_IMPLEMENTED
//#######################################################


static inline void
vm_stack_pack_bool( struct vm_stack *stack,
                    bool v,
                    size_t input_index )
{
    BOOL_PACK_INTO_ARRAY(v, input_index, stack->bits,
                         VM_STACK_MAX_NELEMS, packed_bool);
}

static bool
vm_stack_unpack_bool ( struct vm_stack *stack,
                       size_t input_index )
{
    bool result = BOOL_UNPACK_FROM_ARRAY(input_index, stack->bits,
                                         VM_STACK_MAX_NELEMS, packed_bool);
    return result;
}

static inline size_t
vm_inputs_numelems(struct vm_inputs *vmi)
{
    if ( vmi->num_inputs ) {
        return ( vmi->num_inputs - 1 ) / ( sizeof(packed_bool) * 8)
            + 1;
    } else {
        return 0;
    }
}
static inline size_t
vm_inputs_size(struct vm_inputs *vmi)
{
    return vm_inputs_numelems(vmi) * sizeof(packed_bool);
}


void
token_stack_push(struct token_stack *stack,
                     Token *token)
{
    assert(stack->num_tokens != TOKEN_STACK_MAX_TOKENS_COUNT);
    stack->tokens[(stack->num_tokens) ++] = *token;
}


Token*
token_stack_peek_addr(struct token_stack *stack)
{
    assert(stack->num_tokens);
    return &(stack->tokens[stack->num_tokens - 1]);
}

void
token_stack_pop(struct token_stack *stack)
{
    assert(stack->num_tokens);
    (stack->num_tokens) --;
}

Token
token_stack_pop_value(struct token_stack *stack)
{
    assert(stack->num_tokens);
    Token result = stack->tokens[--(stack->num_tokens)];
    return result;
}



// Highly discouraged pointer may point to invalid
// memory after a new push
Token*
token_stack_pop_value_addr(struct token_stack *stack)
{
    assert(stack->num_tokens);
    Token *result = &(stack->tokens[--(stack->num_tokens)]);
    return result;
}


void
vm_stack_push(struct vm_stack *stack,
                           bool v)
{
    assert(stack->num_bits != VM_STACK_MAX_NUMBITS);
    vm_stack_pack_bool(stack, v, (stack->num_bits));
    (stack->num_bits)++;
}


bool
vm_stack_peek_value(struct vm_stack *stack)
{
    assert(stack->num_bits);
    bool result = vm_stack_unpack_bool(stack, (stack->num_bits) - 1);
    return result;
}

void
vm_stack_pop(struct vm_stack *stack)
{
    assert(stack->num_bits);
    --(stack->num_bits);
}

bool
vm_stack_pop_value(struct vm_stack *stack)
{
    assert(stack->num_bits);
    --(stack->num_bits);
    bool result = vm_stack_unpack_bool(stack, stack->num_bits);
    return result;
}



void
ast_clear( struct ast *ast )
{
    assert(ast);
    ast->num_tokens = 0;
}

#define ast_for( iterator, ast, token)        \
    for (((iterator) = 0), ((token) = (ast).tokens);      \
         ((iterator) < (ast).num_tokens);                 \
         ((token) = & ((ast).tokens[++(iterator)])))      \
        if (true)

#define ast_for_bwd( iterator, ast, token)                          \
    for (( (iterator) = ((ast).num_tokens - 1)),                    \
             ((token) = (ast).tokens + (ast).num_tokens - 1);     \
         (iterator) >= 0;                                             \
         ((token) = & ((ast).tokens[--(iterator)])))                \
        if (true)

#define ast_symtable_for(iterator, symtable, key, value)      \
    stb_sdict_for((symtable)->dict, (iterator), (key), value) \

void
ast_push(struct ast *ast,
         Token *token)
{
    assert(ast->num_tokens != AST_MAX_TOKENS_COUNT);
    ast->tokens[(ast->num_tokens) ++] = *token;
}



static inline bool
symtable_is_valid(struct symtable *symtable)
{
    assert(symtable);
    return (symtable->dict != NULL);
}

bool
symtable_new(struct symtable *symtable)
{
    symtable->dict = stb_sdict_new(1);
    bool alloc_result = symtable->dict != NULL;
    return alloc_result;
}

void symtable_delete(struct symtable *symtable)
{
    assert(symtable);
    if ( symtable_is_valid(symtable) ) {
        stb_sdict_delete(symtable->dict);
        symtable->dict = NULL;
    }
}


// Since the symtables are allocated using arena
// to free all of them we newe to delete
// the entire dictionary.
bool
symtable_clear( struct symtable *symtable )
{
    assert(symtable);
    if ( symtable_is_valid(symtable) ) {
        symtable_delete(symtable);
        bool alloc_result = symtable_new(symtable);
        return alloc_result;
    }
    return true;
}


void
vm_stack_clear( struct vm_stack *vms )
{
    assert(vms);
    vms->num_bits = 0;
}

size_t
symtable_num_ids ( struct symtable *symtable )
{
    if (symtable_is_valid(symtable)) {
        int result = stb_sdict_count(symtable->dict);
        assert(result >= 0);
        return (size_t) result;
    } else {
        return 0;
    }
}

void
symtable_add_identifier(struct symtable *symtable,
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


void
symtable_set_identifier_value( struct symtable *symtable,
                               char   *id_name,
                               size_t  value)
{
    assert(symtable);
    assert(symtable_is_valid(symtable));
    void *value_ = (void*) value;
    stb_sdict_set(symtable->dict, id_name, value_);
}

size_t
symtable_get_identifier_value( struct symtable *symtable,
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
vm_inputs_init_from_symtable(struct vm_inputs *vmi,
                             struct symtable *symtable)
{
    assert(symtable_num_ids(symtable) < VM_INPUTS_MAX_NUMBITS);
    vmi->num_inputs = symtable_num_ids(symtable);
    size_t size = vm_inputs_size(vmi);
    memset(vmi->inputs, 0, size);
}


void
vm_inputs_pack_bool( struct vm_inputs *vmi,
                     bool value,
                     size_t input_index )
{
    assert(input_index < vmi->num_inputs );
# if __DEBUG
    size_t index = input_index / ( sizeof(packed_bool) * 8);
    size_t mask = vmi->inputs[index] & ~((size_t)1 << ((size_t)input_index - index * sizeof(packed_bool) * 8));
    vmi->inputs[index] = mask | (size_t)value << ((size_t)input_index - index * sizeof(packed_bool) * 8);

    { // TODO: The above code has moved in the BOOL_PACK_IN_ARRAY define
        // should check that both the ways to do it are identical
        packed_bool temp = vmi->inputs[index];
#endif
        BOOL_PACK_INTO_ARRAY( value, input_index, vmi->inputs,
                            vm_inputs_numelems(vmi),
                            packed_bool );
#if __DEBUG
        assert ( temp == (input_index / ( sizeof(packed_bool) * 8)));
    }
#endif
    
#if 0
    printf("bi: %zu | index: %zx | mask: %zx | vmi->inputs[index]: %zx\n",
           input_index, index, mask, vmi->inputs[index]);
#endif
}


bool
vm_inputs_unpack_bool( struct vm_inputs *vmi,
                             size_t input_index)
{
    assert(input_index < vmi->num_inputs );
    bool result;
#if __DEBUG
    size_t index = input_index / ( sizeof(packed_bool) * 8);
    size_t local_input_index = ((size_t)input_index - index * sizeof(packed_bool) * 8);
    size_t mask = ( (size_t) 1 << local_input_index);
    result = (vmi->inputs[index] &  mask) >> local_input_index;
#endif
    
    bool temp = BOOL_UNPACK_FROM_ARRAY(input_index, vmi->inputs,
                                       vm_inputs_numelems(vmi),
                                       packed_bool);
#if __DEBUG
    assert(result == temp);
#endif
    result = temp;

    
#if 0
        printf("bi: %zu | index: %zx | local_input_index: %zd | mask: %zx | result: %d\n",
               input_index, index, local_input_index, mask, result);
#endif

    return result;
}

void
vm_inputs_increment(struct vm_inputs *vmi )
{
    assert(vmi);
    assert(vmi->inputs);

    packed_bool *array= vmi->inputs;
    size_t bits_count = vmi->num_inputs;
    
    size_t nelems = vm_inputs_numelems(vmi);
    size_t bits_count_remainder;
    if ( nelems == 0 ) { bits_count_remainder = 0; }
    else {  bits_count_remainder = ( bits_count) % ( sizeof(packed_bool) * 8); }

    for ( packed_bool *s = array; s < array + nelems; s ++) {
        // Needs to check for bubble up
        if ( s == (array + nelems - 1)) {
            packed_bool checkvalue = ((packed_bool) 0 - 1) >> ( sizeof(packed_bool) * 8 - bits_count_remainder);
                
            if ( *s == checkvalue ) {
                // Overflow simulation
                assert(sizeof(array[nelems]) == vm_inputs_size(vmi));
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
vm_inputs_dbglog(struct vm_inputs *vmi)
{
    size_t nelems = vm_inputs_numelems(vmi);
    for ( size_t i = 0; i < nelems; i ++ ) {
        printf(" %zx |", (size_t)vmi->inputs[i]);
    }
    printf("\n");
}


void
vm_stack_dbglog(struct vm_stack *vm)
{
    printf("VM_STACK_DBG_LOG: ");
    for ( size_t i = 0; i < vm->num_bits; i ++ ) {
        bool v = vm_stack_unpack_bool(vm, i);
        printf(" %d |", v);
    }
    printf("\n");
}


#endif /* INTERPRETER_UTILS_C_IMPL */

