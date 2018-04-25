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


#include "ast.h"
#include "ast_node_stack.h"

struct symbol_info {
    bool has_value_assigned;
    bool value;
    size_t vmi_index; // bit index in the vm_inputs associated with this symbol
};

struct symtable {
    stb_sdict *dict; // the name of the symbol will be stored in the dictionary
    // the dictionary stores as a key the symbol name,
    // and as a value the pointer inside the following stack
    // that determines additional infos
#define SYMTABLE_STACK_MAX_SYMBOLS_COUNT 1024
    size_t num_syms;
    struct symbol_info syms[SYMTABLE_STACK_MAX_SYMBOLS_COUNT];
};

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


enum vm_outputs_cache {
    VM_OUTPUT_UKNOWN = 0,
    VM_OUTPUT_ALL_TRUE = 1,
    VM_OUTPUT_ALL_FALSE = 2,
};

struct vm_outputs {
    // Flexible array member
    size_t num_rows;        // 2 ^ n where n is the number of inputs
    size_t num_cols;        // Every cols is the ouput of an applied operator
    enum vm_outputs_cache *row_cache;
    enum vm_outputs_cache *col_cache;
    packed_bool *outputs;   // 2D array of results
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
ast_node_stack_dbglog(struct ast_node_stack *stack)
{
    assert(stack);
    printf("{ stack->num_nodes = %zu", ast_node_stack_num_nodes(stack));
    for ( struct ast_node *it = ast_node_stack_begin(stack);
          it != ast_node_stack_end(stack);
          it ++ ) {
        printf(", [\"");
        ast_node_print(stdout, it);
        printf("\"]{%d}", it->num_operands);
    }
    printf(" }\n");
}


void
vm_stack_push( struct vm_stack *stack,
               bool v )
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



#define ast_symtable_for(iterator, symtable, key, value)         \
    stb_sdict_for((symtable)->dict, (iterator), (key), value)    \

void
ast_dbglog(struct ast* ast)
{
    assert(ast);

    printf("{ ast->num_nodes = %zu\n    ##[Higher-Index] - - - [i] - - - [0]##\n  ", ast_num_nodes(ast));
    for ( struct ast_node *it = ast_end(ast) - 1;
          it >= ast_begin(ast);
          it -- ) {
        printf("[\"");
        ast_node_print(stdout, it);
        printf("\"]{%d}, ", it->num_operands);
    }
    printf(" }\n");
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
    symtable->num_syms = 0;
    return alloc_result;
}

void symtable_delete(struct symtable *symtable)
{
    assert(symtable);
    if ( symtable_is_valid(symtable) ) {
        stb_sdict_delete(symtable->dict);
        symtable->dict = NULL;
    }
    symtable->num_syms = 0;
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
    symtable->num_syms = 0;
    return true;
}


size_t
symtable_num_syms ( struct symtable *symtable )
{
    if (symtable_is_valid(symtable)) {
        int result = stb_sdict_count(symtable->dict);
        size_t numsyms = symtable->num_syms;
        assert((size_t)((int)numsyms) == numsyms);
        assert(result >= 0);
        assert(numsyms == (size_t)result);
        assert(result >= 0);
        return (size_t) result;
    } else {
        return 0;
    }
}

struct symbol_info *
symtable_get_syminfo( struct symtable *symtable,
                      char *sym_name, size_t sym_name_len)
{
    char temp = sym_name[sym_name_len];
    sym_name[sym_name_len] = 0;
    void *q = stb_sdict_get(symtable->dict, sym_name);
    sym_name[sym_name_len] = temp;
     
    struct symbol_info *info = (struct symbol_info *) q;
    return info;
}

struct symbol_info *
symtable_syminfo_from_node( struct symtable *symtable,
                            struct ast_node *node )
{
    assert(node);
    assert(node->type == AST_NODE_TYPE_IDENTIFIER);
    return symtable_get_syminfo(symtable, node->text, node->text_len);
}

static inline bool
symtable_is_sym(struct symtable *symtable,
                char *sym_name, size_t sym_name_len)
{
    return symtable_get_syminfo(symtable, sym_name, sym_name_len);
}



struct symbol_info *
symtable_add_identifier( struct symtable *symtable,
                         char *string, size_t string_len )
{
    assert(symtable && symtable->dict);
    assert(string);

    assert(symtable->num_syms < SYMTABLE_STACK_MAX_SYMBOLS_COUNT);

    // Not already inside the symtable

    // null terminate before adding
    char temp = string[string_len];
    string[string_len] = 0;

    // assert symbol is not already present inside the symbol table
    assert(symtable_is_sym(symtable, string, string_len) == false);
    


    struct symbol_info *result = & symtable->syms[(symtable->num_syms)];
    symtable->syms[(symtable->num_syms) ++ ] = (struct symbol_info) { 0 };
    stb_sdict_set(symtable->dict, string, result);

    // restore null termination
    string[string_len] = temp;

    
    return result;
}



// Returns bool if it has a value assigned
// and populates *value with the corresponding assigned value
// If no value is assigned it returns false and *value is not
// guaranteed to contain legit data (btw it get's cleared to zero)
bool
ast_node_value_assigned(struct symtable *symtable,
                        struct ast_node *node,
                        bool *value)
{
    assert(node->type == AST_NODE_TYPE_IDENTIFIER);
        
    struct symbol_info *info =
        symtable_get_syminfo( symtable,
                              node->text, node->text_len);
    
    bool result;
    if ( info ) {
        result = true;
        *value = info->value;
    } else {
        result = false;
        *value = 0;
    }
    return result;

}


void
vm_inputs_init_from_symtable(struct vm_inputs *vmi,
                             struct symtable *symtable)
{
    assert(symtable_num_syms(symtable) < VM_INPUTS_MAX_NUMBITS);
    vmi->num_inputs = symtable_num_syms(symtable);
    size_t size = vm_inputs_size(vmi);
    memset(vmi->inputs, 0, size);
}


void
vm_inputs_assign_value( struct vm_inputs *vmi,
                        bool value,
                        size_t input_index )
{
    assert(input_index < vmi->num_inputs );
    BOOL_PACK_INTO_ARRAY( value, input_index, vmi->inputs,
                          vm_inputs_numelems(vmi),
                          packed_bool );
}


bool
vm_inputs_get_value( struct vm_inputs *vmi,
                     size_t input_index )
{
    assert(input_index < vmi->num_inputs );
    bool result = BOOL_UNPACK_FROM_ARRAY(input_index, vmi->inputs,
                                       vm_inputs_numelems(vmi),
                                       packed_bool);
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
vm_inputs_clear(struct vm_inputs *vmi)
{
    size_t size = vm_inputs_size(vmi);
    memset(vmi->inputs, 0, size);
    vmi->num_inputs = 0;
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



static inline size_t
vm_outputs_numelems(struct vm_outputs *vmo)
{
    size_t nums = (vmo->num_rows) * (vmo->num_cols);
    if ( nums ) {
        return ( nums - 1 ) / ( sizeof(packed_bool) * 8)
            + 1;
    } else {
        return 0;
    }
}


static inline size_t
vm_outputs_size(struct vm_outputs *vmo)
{
    return vm_outputs_numelems(vmo) * sizeof(packed_bool);
}

static inline size_t
vm_outputs_get_bitindex( struct vm_outputs *vmo,
                         size_t row, size_t col )
{
    assert(vmo);
    return (col + row * vmo->num_cols);
}

void
vm_outputs_pack_bool( struct vm_outputs *vmo,
                      bool value,
                      size_t row, size_t col )
{
    assert(vmo);
    assert(vmo->outputs);
    assert(vmo->row_cache);
    assert(vmo->col_cache);
    assert(row < vmo->num_rows);
    assert(col < vmo->num_cols);

    size_t bitindex = vm_outputs_get_bitindex(vmo, row, col);

    size_t nelems = vm_outputs_numelems(vmo);
    
    BOOL_PACK_INTO_ARRAY(value, bitindex, vmo->outputs,
                         nelems, packed_bool);
}


bool
vm_outputs_unpack_bool( struct vm_outputs *vmo,
                        size_t row, size_t col)
{
    assert(vmo);
    assert(vmo->outputs);
    assert(vmo->row_cache);
    assert(vmo->col_cache);
    assert(row < vmo->num_rows);
    assert(col < vmo->num_cols);

    size_t bitindex = vm_outputs_get_bitindex(vmo, row, col);

    size_t nelems = vm_outputs_numelems(vmo);
    
    bool b = BOOL_UNPACK_FROM_ARRAY(bitindex, vmo->outputs,
                                    nelems, packed_bool);
    return b;
}


void
vm_outputs_set_row_cache( struct vm_outputs *vmo,
                          size_t row,
                          enum vm_outputs_cache c)
{
    assert(vmo);
    assert(vmo->outputs);
    assert(vmo->row_cache);
    assert(vmo->col_cache);

    assert(row < vmo->num_rows);
    vmo->row_cache[row] = c;
}

void
vm_outputs_set_col_cache( struct vm_outputs *vmo,
                          size_t col,
                          enum vm_outputs_cache c)
{
    assert(vmo);
    assert(vmo->outputs);
    assert(vmo->row_cache);
    assert(vmo->col_cache);

    assert(col < vmo->num_cols);
    vmo->col_cache[col] = c;
}

#if 0
void
vm_outputs_dbglog(struct vm_outputs *vmo)
{
    size_t nelems = vm_ouputs_numelems(vmo);
    printf("Vm Outputs Cols Cached:\n");
    for ( size_t i = 0; i < vmo->num_cols; i++ ) {
        bool b = vm_outputs_unpack_bool( vmo, )

        printf(" %b |", b);
    }
    
    for ( size_t i = 0;_ i < nelems; i ++ ) {
        printf(" %zx |", (size_t)vmi->outputs[i]);
    }
    printf("\n");
}
#endif



void
vm_stack_clear( struct vm_stack *vms )
{
    assert(vms);
    vms->num_bits = 0;
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



/* HASHING FUNCTIONS */

size_t
string_compute_hash(char *str)
{
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


typedef long long int hash_node_val;

struct hash_node {
    struct hash_node *next;
    char *key;
    hash_node_val value;
};


struct hash_map {
    #define HASH_MAP_MAX_NELEMS 1024
    struct hash_node *nodes[HASH_MAP_MAX_NELEMS]; // linked list nodes
};


bool
hash_map_init( struct hash_map *hm )
{
    struct hash_node node;
    bool result = true;
    memset(hm, 0, sizeof(struct hash_map));
    return result;
}


bool
hash_map_add( struct hash_map *hm,
              char *key,
              hash_node_val val )
{
    bool result = true;
    size_t hash = string_compute_hash(key);
    size_t index = hash % HASH_MAP_MAX_NELEMS;

    struct hash_node **node = &(hm->nodes[index]);

    while( (*node)->next != NULL) {
        node = &((*node)->next);
    }

    struct hash_node *t = malloc(sizeof(struct hash_node));
    if ( !t ) {
        (*node)->next = t;
        char *tempstring = strdup(key);
        if ( tempstring ) {
            (*node)->key = key;
            (*node)->value = val;
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    
    return result;
}
              




#endif /* INTERPRETER_UTILS_C_IMPL */

