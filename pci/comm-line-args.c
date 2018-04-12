#ifndef COMM_LINE_ARGS_C_INCLUDE
#define COMM_LINE_ARGS_C_INCLUDE


#include <argp.h>
#include <sysexits.h>

//#######################################################
#endif /* COMM_LINE_ARGS_C_INCLUDE */
#if !defined COMM_LINE_ARGS_C_IMPLEMENTED && defined COMM_LINE_ARGS_C_IMPL
#define COMM_LINE_ARGS_C_IMPLEMENTED
//#######################################################


const char *argp_program_version = "0.1";
error_t argp_error_exit_status = EX_USAGE;


#define SOLVER_ARGP_KEY 's'

static const struct argp_option opts[] = {
    // name, key, arg, flags, doc, group
    { "solver", SOLVER_ARGP_KEY, "SOLVER", OPTION_ARG_OPTIONAL | 0, "Specify the solver to use: {bruteforce | SLD}", 1},
    {0}
};

error_t parser (int key, char *arg, struct argp_state *state)
{
    if ( key == ARGP_KEY_INIT ) {
        // This is always the first time this function will get called
        // can do the initialization here
    } if ( key == ARGP_KEY_NO_ARGS ) {
        // No argument passed to the program
    } else if ( key == ARGP_KEY_END ) {
        // End of parsing
    } else if ( key == ARGP_KEY_SUCCESS ) {
        // Command line reading was successfull
    } else if ( key == ARGP_KEY_FINI ) {
        // Final time this function will ever get called
        // can do free's here
    } else {
        switch (key) {

        case SOLVER_ARGP_KEY: {
            printf("Setting the solver to: %s\n", arg);
        } break;
            
        default: {
            return ARGP_ERR_UNKNOWN;
        } break;
        }

    }

    
    return 0;
}

static const struct argp argp = {
    .options = opts,
    .parser = parser,
    .doc = NULL,
    .children = NULL,
    .argp_domain = NULL
};



void
comm_line_args(int argc, char **argv)
{

    int *arg_index = NULL;
    void *input = NULL;
    argp_parse(& argp, argc, argv, 0, arg_index, input);
}




#endif /* COMM_LINE_ARGS_C_IMPL */
