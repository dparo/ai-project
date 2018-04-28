#ifndef MAIN_C_INCLUDE
#define MAIN_C_INCLUDE

//#######################################################
#endif /* MAIN_C_INCLUDE */
#if !defined MAIN_C_IMPLEMENTED && defined MAIN_C_IMPL
#define MAIN_C_IMPLEMENTED
//#######################################################


#define UTILS_C_IMPL
#include "utils.c"

#define PLATFORM_C_IMPL
#include "platform.c"

#include <stdlib.h>
#include <stdio.h>


#define STB_DEFINE
#include <stb.h>


#include <stdlib.h>
#include <stdio.h>



static void
fatal(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(-1);
}


#define MEMORY_C_IMPL
#include "memory.c"


#define TOKENIZER_C_IMPL
#include "tokenizer.c"


char *
load_file_null_terminate(FILE *f, size_t *file_size_out)
{
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = xmalloc(size + 1); // Note 1 for null termination
    assert(buffer);
    size_t readsize = fread(buffer, 1, size, f);
    if ( readsize != size ) {
        fatal("Fatal error when reading file: File is %zu bytes big, but %zu bytes was read\n", size, readsize);
    }
    *file_size_out = size;
    buffer[size] = 0;
    return buffer;
}


void
log_token(Token *token, FILE *f)
{
    fprintf(f, "%.*s", token->text_len, token->text);
}

enum meta_replacement_rule_type {
    META_REPLACEMENT_RULE_NONE = 0,
    META_REPLACEMENT_RULE_STACK = 0,
};

struct meta_replacement_rule {
    enum meta_replacement_rule_type type;

    // The rule that gets passed is the equivalent of `this` or `self`
    void (*fn_replacement_rule)(FILE *f, Token *token, struct meta_replacement_rule *rule);

    
    union {
        // Stack replacement rule
        struct {
            char *stack_name;
            char *stack_node_type_name;
            bool stack_push_pointer;
            char *stack_base_pointer_name;
            char *stack_num_elems_name;
            char *stack_max_elems_name;
        };
    };
};


struct meta_generate_infos_internal {
    bool file_already_opened;
};


struct meta_generate_infos {
    struct meta_replacement_rule rule;
    char *input_path;
    char *output_path;
    struct meta_generate_infos_internal mgii; // Reserved CLEAR TO ZERO
};

bool
match_token(Token *token,
            Token_Type type,
            char *string )
{
    if ( token->type != type ) return false;
    return strncmp(string, token->text, token->text_len) == 0;
}


bool
match_meta_token(Token *t,
                 char *string )
{
    return match_token(t, TT_META, string);
}

void
print_replaced_param(FILE *f,
                     Token *token,
                     char *replacer,
                     char *string_if_missing)
{
    char *string;
    bool print_token = false;
    if ( replacer ) {
        string = replacer;
    } else if (string_if_missing) {
        string = string_if_missing;
    } else {
        string = token->text;
        print_token = true;
    }

    if (print_token) {
        log_token(token, f);
    } else {
        fputs(string, f);
    }
}


void stack_template_replace( FILE *f,
                             Token *token,
                             struct meta_replacement_rule *rule)
{
    if (match_meta_token(token, "S")) {
        print_replaced_param(f, token, rule->stack_name, "`MISSING NAME`");
    } else if (match_meta_token(token, "T")) {
        print_replaced_param(f, token, rule->stack_node_type_name, "`MISSING TYPE`");
    } else if (match_meta_token(token, "ref")
               || match_meta_token(token, "deref")) {
        if (rule->stack_push_pointer == true ) {
            print_replaced_param(f, token, "*", "`CODE GEN: ERROR`");
        } else {
            print_replaced_param(f, token, "", "`CODE GEN: ERROR`");
        }
    } else if (match_meta_token(token, "base")) {
        print_replaced_param(f, token, rule->stack_base_pointer_name, "base");
    } else if (match_meta_token(token, "num_elems")) {
        print_replaced_param(f, token, rule->stack_num_elems_name, "num_elems");
    } else if (match_meta_token(token, "max_elems")) {
        print_replaced_param(f, token, rule->stack_max_elems_name, "max_elems");
    } else {
        log_token(token, f);
    }

}


struct meta_generate_infos mgi[] =
{
    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,
       .stack_name = "ast",
       .stack_node_type_name = "struct ast_node",
       .stack_push_pointer = true,
       .stack_base_pointer_name = "nodes",
       .stack_num_elems_name = "num_nodes",
       .stack_max_elems_name = "max_nodes"},
      "code-gen/templates/stack.template.c", "__generated__/ast.h",
      {0}
    },

    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,

       .stack_name = "ast_node_stack",
       .stack_node_type_name = "struct ast_node",
       .stack_push_pointer = true,
       .stack_base_pointer_name = "nodes",
       .stack_num_elems_name = "num_nodes",
       .stack_max_elems_name = "max_nodes"},
      "code-gen/templates/stack.template.c", "__generated__/ast_node_stack.h",
      {0}
    },

    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,

       .stack_name = "ast_node_ref_stack",
       .stack_node_type_name = "struct ast_node *",
       .stack_push_pointer = true,
       .stack_base_pointer_name = "nodes",
       .stack_num_elems_name = "num_nodes",
       .stack_max_elems_name = "max_nodes"},
      "code-gen/templates/stack.template.c", "__generated__/ast_node_stack_ref.h",
      {0}
    },
    
    
    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,
       .stack_name = "uint32_stack",
       .stack_push_pointer = false,
       .stack_node_type_name = "uint32_t",
       .stack_base_pointer_name = "data",
       .stack_num_elems_name = "num_elems",
       .stack_max_elems_name = "max_elems"},
      "code-gen/templates/stack.template.c", "__generated__/uint32-stack.h",
      {0}
    },

    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,
       .stack_name = "ast_node_child_parent_pair_stack",
       .stack_push_pointer = true,
       .stack_node_type_name = "struct ast_node_child_parent_pair",
       .stack_base_pointer_name = "pairs",
       .stack_num_elems_name = "num_pairs",
       .stack_max_elems_name = "max_pairs"},
      "code-gen/templates/stack.template.c", "__generated__/ast-node-child-parent-pair-stack.h",
      {0}
    },
    
    {0}
};


void
mgi_execute(void);


int
main (int argc, char ** argv)
{
    platform_init();
    mgi_execute();
# if 0
    Tokenizer tknzr;
    Token token = Empty_Token;

    tokenizer_init_with_memmapped_file(&tknzr, "code-gen/templates/stack.template.c");

    
    while( get_next_token(& tknzr, & token)) {
        if ( tknzr.err ) {
            fatal("Tokenizer parsing error: %s\n", tknzr.err_desc);
        }
    }
#endif
    return 0;
}

static FILE *
mgi_output_file_handle(char *path)
{
    struct meta_generate_infos *it = mgi;
    for (it = mgi; it->output_path != 0; it++) {
        if ( strcmp(path, it->output_path) == 0 ) {
            if ( it->mgii.file_already_opened) {
                return fopen(path, "a"); // already opened append to it
            } else {
                it->mgii.file_already_opened = true;
                return fopen(path, "w");
            }
        }
    }
    return NULL;
}



void
mgi_execute(void)
{
    struct meta_generate_infos *it = mgi;
    for (it = mgi; it->input_path != 0; it++) {
        Tokenizer tknzr = {0};
        Token token = Empty_Token;
        
        if ( !it->input_path )
            goto next_it;
        
        FILE *input_file = fopen(it->input_path, "r");

        if (!input_file) {
            fprintf(stderr, "CODE-GEN: Failed reading input file: %s\n", it->input_path);
            goto next_it;
        }

        if (!it->output_path) {
            fprintf(stderr, "CODE-GEN: Input file %s, has no associated output file\n", it->input_path);
            goto next_it;
        }
        
        FILE *output_file = mgi_output_file_handle(it->output_path);
        if ( !output_file ) {
            fprintf(stderr, "CODE-GEN: Output file %s failed to open\n", it->output_path);
            goto next_it;
        }


        if (!it->rule.fn_replacement_rule) {
            fprintf(stderr, "CODE-GEN: Warning `{}` has no valid replacement rule function pointer\n...Skipping\n ");
            goto next_it;
        }
        
        size_t buffer_len;
        char *buffer = load_file_null_terminate(input_file, &buffer_len);
            
        tokenizer_init_from_memory(&tknzr, buffer, buffer_len);
    
        while( get_next_token(& tknzr, & token)) {
            if ( tknzr.err ) {
                fatal("Tokenizer parsing error: %s\n", tknzr.err_desc);
            }
            it->rule.fn_replacement_rule(output_file, & token, &(it->rule));
        }

    next_it:
        if (buffer)
            free(buffer);
        if (input_file)
            fclose(input_file);
        if (output_file)
            fclose(output_file);
    }

}




#endif /* MAIN_C_IMPL */

