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
load_file_null_terminate(char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fatal("Could not open file %s\n", path);
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = xmalloc(size + 1); // Note 1 for null termination
    assert(buffer);
    size_t readsize = fread(buffer, 1, size, f);
    if ( readsize != size ) {
        fatal("Fatal error when reading file: File is %zu bytes big, but %zu bytes was read\n", size, readsize);
    }
    buffer[size] = 0;
    return buffer;
}


void
log_token(Token *token)
{
    printf("%.*s", token->text_len, token->text);
}

int
main (int argc, char ** argv)
{
    platform_init();
    Tokenizer tknzr;
    Token token = Empty_Token;
    tokenizer_init_with_memmapped_file(&tknzr, "code-gen/templates/stack.template.c");
    
    while( get_next_token(& tknzr, & token)) {
        if ( tknzr.err ) {
            fatal("Tokenizer parsing error: %s\n", tknzr.err_desc);
        }
        if ( token.type == TT_META ) {
            if (strncmp("S", token.text, token.text_len) == 0 ) {
                printf("ast");
            } else if (strncmp("T", token.text, token.text_len) == 0 ) {
                printf("ast_node");
            } else {
                log_token( &token);
            }
        } else {
            log_token(& token);
        }
    }
    return 0;
}




#endif /* MAIN_C_IMPL */
