#ifndef MEMORY_C_INCLUDE
#define MEMORY_C_INCLUDE


//#######################################################
#endif /* MEMORY_C_INCLUDE */
#if !defined MEMORY_C_IMPLEMENTED && defined MEMORY_C_IMPL
#define MEMORY_C_IMPLEMENTED
//#######################################################


#include <stdlib.h>



void *
xmalloc(size_t size)
{
    void *result = malloc(size);
    if ( !result )
        perror("MEM: Memory allocation failed");
    return result;
}

void *
xrealloc(void *ptr, size_t newsize)
{
    void *result = realloc(ptr, newsize);

    if (!result) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "MEM: Failed to reallocate %zu bytes of memory", newsize);
        perror(err_msg);
    }
    return result;
}



#endif /* MEMORY_C_IMPL */
