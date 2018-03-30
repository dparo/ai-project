#ifndef COMM_LINE_ARGS_C_INCLUDE
#define COMM_LINE_ARGS_C_INCLUDE


#include "utils.c"

//#######################################################
#endif /* COMM_LINE_ARGS_C_INCLUDE */
#if !defined COMM_LINE_ARGS_C_IMPLEMENTED && defined COMM_LINE_ARGS_C_IMPL
#define COMM_LINE_ARGS_C_IMPLEMENTED
//#######################################################







#define COMM_LINE_ARG_VALUE_MAX_STRING_SIZE 1024
// command line interface option
typedef struct Comm_Line_Arg {
     enum Comm_Line_Arg_Type {
          COMM_LINE_ARG_NONE = 0,
          COMM_LINE_ARG_BOOL,
          COMM_LINE_ARG_TOGGLE,

          COMM_LINE_ARG_INT,
          COMM_LINE_ARG_FLOAT,
          COMM_LINE_ARG_STRING,

     } type;


     char id[256];
     bool found;

     union {
          bool b;
          int i;
          float f;
          char string[COMM_LINE_ARG_VALUE_MAX_STRING_SIZE];
          bool toggled;
     } value;
     
} Comm_Line_Arg;
typedef enum Comm_Line_Arg_Type Comm_Line_Arg_Type;

static const Comm_Line_Arg Empty_Comm_Line_Arg = {0};














static char *
_search_comm_line_string_for_equal_sign(char *string)
{
     int eq_it;
     char *result = 0x0;
     for(eq_it = 0; string[eq_it] != 0; eq_it++) {
          if (string[eq_it] == '=') {
               result = &string[eq_it];
               break;
          }
     }
     return result;
}

static inline bool
_is_comm_line_arg_toggle_string(char *string)
{
     if(string[0] == '-') {
          if (string[1] == '-') {
               return true;
          }
     }
     return false;
}

static inline void
_strncpy_comm_line_arg_until_space( char *src, char *dest)
{
     i32 size = COMM_LINE_ARG_VALUE_MAX_STRING_SIZE;
     i32 it;
     for (it = 0; (it < size) && (src[it] != 0); it++) {
          if (src[it] == ' ') {
               break;
          }
          dest[it] = src[it];
     }
}




static inline bool
_strncmp_comm_line_arg_until_space_or_equal( char *s1, char *s2 )
{
     bool result = true;
     i32 size = COMM_LINE_ARG_VALUE_MAX_STRING_SIZE;
     i32 i;
     for(i = 0; (i < size) && (s1[i] != 0) && (s2[i] != 0); i++) {
          if (s1[i] == ' ' || s2[i] == ' ') {
               break;
          }
          if (s1[i] == '=' || s2[i] == '=') {
               break;
          }
          if (s1[i] != s2[i] ) {
               result = false;
               break;
          }
     }

     return result;
}

static inline void
_output_comm_line_arg_value(Comm_Line_Arg *opt, char *string )
{
     switch(opt->type) {
     case COMM_LINE_ARG_STRING: {
          _strncpy_comm_line_arg_until_space(string, opt->value.string);
     } break;

     default: {
          // needs implementation or wrong expected type found in the thing
          invalid_code_path();
     } break;

     }
     opt->found = true;
}


int
parse_comm_line_args(Comm_Line_Arg *args, int argc, char** argv)
{
     i32 it = 1;
     for( it = 1; it < argc; ++it ) {
          i32 args_it;
          char *eq_str = _search_comm_line_string_for_equal_sign(argv[it]);
          bool is_comm_line_toggle = _is_comm_line_arg_toggle_string(argv[it]);

          for(args_it = 0; args[args_it].id != NULL; ++args_it) {
               Comm_Line_Arg *arg = & args[args_it];
               if (arg->found == false ) {
                    assert(arg->id != NULL);

                    if ( eq_str ) {
                         if ( _strncmp_comm_line_arg_until_space_or_equal(argv[it], arg->id)) {
                              // TODO: Give actual usefull value back
                              _output_comm_line_arg_value(arg, eq_str + 1);
                              break;
                         }
                    }
                    else if ( is_comm_line_toggle ) {
                         if ( _strncmp_comm_line_arg_until_space_or_equal(argv[it], arg->id)) {
                              assert(arg->type == COMM_LINE_ARG_TOGGLE);
                              arg->value.toggled = true;
                              arg->found = true;
                              break;
                         }

                    }

               }
          }
     }
     return it;
}






#endif /* COMM_LINE_ARGS_C_IMPL */
