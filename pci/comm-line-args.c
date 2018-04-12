#ifndef COMM_LINE_ARGS_C_INCLUDE
#define COMM_LINE_ARGS_C_INCLUDE


#include <argp.h>

//#######################################################
#endif /* COMM_LINE_ARGS_C_INCLUDE */
#if !defined COMM_LINE_ARGS_C_IMPLEMENTED && defined COMM_LINE_ARGS_C_IMPL
#define COMM_LINE_ARGS_C_IMPLEMENTED
//#######################################################


const char *argp_program_version = "0.1";

void
prova(int argc, char **argv)
{
    const struct argp *argp;
    int *arg_index = NULL;
    void *input = NULL;
    argp_parse(argp, argc, argv, 0, arg_index, input);
}




#endif /* COMM_LINE_ARGS_C_IMPL */
