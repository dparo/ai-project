/*
 * Copyright (C) 2018  Davide Paro
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
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


static const struct argp_option opts[] = {
    // name, key, arg, flags, doc, group

    // @NOTE: the group is used to sort and show near options of the same group
    // when requesting the --help option
    { "solver", 's', "SOLVER", OPTION_ARG_OPTIONAL | 0, "Specify the solver to use: {bruteforce | dpll | tp}", 1},
    {0}
};

static inline void
commline_dbglog(const char *fmt, ...)
{
#if 0
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#endif
}

static bool solver_was_set = false;

void
commline_set_solver(char *solver)
{
    if ( solver ) {
        if ((0 == strcasecmp("bf", solver))
            || (0 == strcasecmp("bruteforce", solver))) {
            interpreter_logi("Setting the solver to: `BRUTEFORCE`\n");
            interpreter_set_solver(BRUTEFORCE_SOLVER);
            solver_was_set = true;
        } else if (0 == strcasecmp("dpll", solver)) {
            interpreter_logi("Setting the solver to: `DPLL`\n");
            interpreter_set_solver(DPLL_SOLVER);
            solver_was_set = true;
        } else if ((0 == strcasecmp("th", solver))
                   || (0 == strcasecmp("tp", solver))
                   || 0 == strcasecmp("dpll-th", solver)
                   || 0 == strcasecmp("dpll-tp", solver)) {
            interpreter_logi("Setting the solver to: `THEOREM-PROVER`\n");
            interpreter_set_solver(THEOREM_PROVER);
            solver_was_set = true;
        } else {
            interpreter_logi("INTERPRETER: Cannot understand `solver`\n   > Defaulting to DPLL\n");
            interpreter_set_solver(DPLL_SOLVER);
            solver_was_set = true;
        }
    } else {
        solver_was_set = false;
    }
    
}


error_t
argp_parser (int key, char *arg, struct argp_state *state)
{
    if ( key == ARGP_KEY_INIT ) {
        // This is always the first time this function will get called
        // can do the initialization here
        commline_dbglog("ARGP_KEY_INIT\n");
    } if ( key == ARGP_KEY_NO_ARGS ) {
        // No argument passed to the program
        commline_dbglog("ARGP_KEY_NO_ARGS\n");
    } else if ( key == ARGP_KEY_END ) {
        // End of parsing
        commline_dbglog("ARGP_KEY_END\n");
        if (! solver_was_set ) {
            interpreter_logi("INTERPRETER: Solver was not set\n  > Defaulting to DPLL\n");
            interpreter_set_solver(DPLL_SOLVER);
        }
    } else if ( key == ARGP_KEY_SUCCESS ) {
        // Command line reading was successfull
        commline_dbglog("ARGP_KEY_SUCCESS\n");
    } else if ( key == ARGP_KEY_FINI ) {
        // Last time this function will ever get called
        // can do free's and cleanup's here
        commline_dbglog("ARGP_KEY_FINI\n");
    } else {
        switch (key) {

        case 's': {
            commline_set_solver(arg);
        } break;
            
        default: {
            commline_dbglog("ARGP_ERR_UNKNOWN\n");
            return ARGP_ERR_UNKNOWN;
        } break;
        }

    }

    
    return 0;
}

static const struct argp argp = {
    .options = opts,
    .parser = argp_parser,
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
