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
            char *node_type_name;
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



void stack_template_replace( FILE *f,
                             Token *token,
                             struct meta_replacement_rule *rule)
{
    if ( token->type == TT_META ) {
        if (strncmp("S", token->text, token->text_len) == 0 ) {
            fprintf(f, rule->stack_name);
        } else if (strncmp("T", token->text, token->text_len) == 0 ) {
            fprintf(f, rule->node_type_name);
        } else {
            log_token( token, f);
        }
    } else {
        log_token(token, f);
    }

}


struct meta_generate_infos mgi[] =
{
    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,
       .stack_name = "test1",
       .node_type_name = "struct tms*" },
      "code-gen/templates/stack.template.c", "__generated__/test1.h",
      {0}
    },

    { {.type = META_REPLACEMENT_RULE_STACK,
       .fn_replacement_rule = & stack_template_replace,
       .stack_name = "test2",
       .node_type_name = "struct tms*" },
      "code-gen/templates/stack.template.c", "__generated__/test2.h",
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

