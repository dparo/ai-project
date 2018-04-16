#ifndef PARSER_C_INCLUDE
#define PARSER_C_INCLUDE

#include "utils.c"
#include "platform.c"


#if defined __DEBUG
#define TOKENIZER_DEBUG 1
#else
#define TOKENIZER_DEBUG 0
#endif

#define TOKENIZER_CACHING_ENABLED 1
#define TOKENIZER_STATE_LIST_ENABLED 1

enum Token_Type {
    TT_NONE = 0,
    TT_IDENTIFIER = 1,
    TT_KEYWORD,
    TT_CONSTANT = 3,


    
    TT_PUNCT_EQUAL,
    TT_PUNCT_EQUAL_EQUAL,
    TT_PUNCT_NOT_EQUAL,
    TT_PUNCT_COMMA,

    TT_PUNCT_ARROW,
    TT_PUNCT_BOTHDIR_ARROW,
    TT_PUNCT_DEDUCTION, // |- |=
    TT_PUNCT_DEFINITION, // :-


    TT_PUNCT_LOGICAL_NOT,
    TT_PUNCT_BITWISE_NOT,
    TT_PUNCT_LOGICAL_AND,
    TT_PUNCT_BITWISE_AND,
    TT_PUNCT_LOGICAL_OR,
    TT_PUNCT_BITWISE_OR,
    TT_PUNCT_BITWISE_XOR,


    TT_PUNCT_BITWISE_AND_EQUAL,
    TT_PUNCT_BITWISE_OR_EQUAL,
    TT_PUNCT_BITWISE_XOR_EQUAL,
     

    TT_PUNCT_GREATER,
    TT_PUNCT_LESS,
    TT_PUNCT_GREATER_OR_EQUAL,
    TT_PUNCT_LESS_OR_EQUAL,

    TT_PUNCT_SEMICOLON, // ;


    TT_PUNCT_QUESTION_MARK,
    TT_PUNCT_COLON,     // :


    TT_PUNCT_DOLLAR_SIGN, // $

    TT_PUNCT_AT_SIGN, // @
    TT_PUNCT_POUND, // #



    TT_PUNCT_BACKWARD_SLASH, //
    TT_PUNCT_BACKWARD_APOSTROPHE, // `



    TT_PUNCT_PLUS,
    TT_PUNCT_MINUS,
    TT_PUNCT_ASTERISK,
    TT_PUNCT_DIVISION,
    TT_PUNCT_MODULO,
    TT_PUNCT_INCREMENT,
    TT_PUNCT_DECREMENT,




    TT_PUNCT_BITWISE_LEFT_SHIFT,
    TT_PUNCT_BITWISE_RIGHT_SHIFT,

    TT_PUNCT_ADDITION_EQUAL,
    TT_PUNCT_SUBTRACTION_EQUAL,
    TT_PUNCT_MULTIPLICATION_EQUAL,
    TT_PUNCT_DIVISION_EQUAL,
    TT_PUNCT_MODULO_EQUAL,
    TT_PUNCT_BITWISE_LEFT_SHIFT_EQUAL,
    TT_PUNCT_BITWISE_RIGHT_SHIFT_EQUAL,


    TT_PUNCT_DOT_DOT_DOT,

    TT_PUNCT_DOT,



    TT_PUNCT_OPEN_PAREN,
    TT_PUNCT_CLOSE_PAREN,
    TT_PUNCT_OPEN_BRACKET,
    TT_PUNCT_CLOSE_BRACKET,
    TT_PUNCT_OPEN_BRACE,
    TT_PUNCT_CLOSE_BRACE,

    TT_PUNCT_ENUM_LAST_VALUE,
};
typedef enum Token_Type Token_Type;





typedef struct Token {
    Token_Type type;
    char *text; // Unique ID of the token and contained text
    i32 text_len;
    i32 line_num;
    i32 column;
} Token;
static const Token Empty_Token = {
    TT_NONE,
    "",
    0,
    0,
    0,
};






typedef enum Tokenization_Error {
    TOK_ERR_NONE = 0,
    TOK_ERR_OUT_OF_MEM = 1,
    TOK_ERR_INVALID_STRING = 2,
} Tokenization_Error;


#define TOKENIZER_FILENAME_LEN 256
#define TOKENIZER_ERR_DESC_LEN 512
#define TOKENIZER_STATES_LEN 256



typedef struct Tokenizer {
    char file[TOKENIZER_FILENAME_LEN];

    i32 line_num;
    i32 column;


    char *base; // Must point to content of file + at least 4 bytes of 0 termination
    // and preceded by at least 4 bytes of 0x00
    i64 base_len;

    enum Tokenization_Error err;
    char err_desc[TOKENIZER_ERR_DESC_LEN];

    char *at;
} Tokenizer;



static inline int
token_txt_cmp(Token *t, char *string)
{
    return strncmp(string, t->text, t->text_len);
}


void log_token (Token *token);


bool
token_is_operator(Token *t);


void
tokenizer_init (Tokenizer* tok, char* filename );


bool
get_next_token ( Tokenizer *tknzr,
                 Token *token );

bool
discard_next_token ( Tokenizer* tok );


bool
compare_token_to ( Token *token, char *token_text );

bool
compare_next_token_to (Tokenizer *tknzr, char * token_text );

// #######################################################################
#endif /* PARSER_C_INCLUDE */
#if !defined PARSER_C_IMPLEMENTED && defined PARSER_C_IMPL
#define PARSER_C_IMPLEMENTED
// #######################################################################


#include <stdarg.h>
#include <stddef.h>
#include <memory.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int strcmp(const char *s1, const char *s2);



static inline char
tokenizer_deref_at ( Tokenizer *tknzr,
                     int relative_index )
{
    if ( (tknzr->at + relative_index) >= (tknzr->base + tknzr->base_len)) {
        return 0;
    } else if ( tknzr->at + relative_index < (tknzr->base)) {
        return 0;
    } else {
        return *(tknzr->at + relative_index);
    }
}


static inline char
tokenizer_deref ( Tokenizer *tknzr)
{
    return tokenizer_deref_at(tknzr, 0);
}


static bool
tokenizer_is_end ( Tokenizer *tknzr )
{
    bool result = false;
    if (((tknzr->at) >= (tknzr->base + tknzr->base_len))
        ||( tokenizer_deref(tknzr) == 0)) {
        result = true;
    }
    return result;
}


static inline void
tokenizer_adv_by ( Tokenizer *tknzr, i32 step )
{
    (tknzr->at)     += step;
    tknzr->column   += step;
}


static inline void
tokenizer_adv ( Tokenizer *tknzr )
{
    tokenizer_adv_by(tknzr, 1);
}



static inline void
tokenizer_err_fmt(Tokenizer *tknzr, Tokenization_Error errtype, char* fmt, ...)
{
    tknzr->err = errtype;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tknzr->err_desc, TOKENIZER_ERR_DESC_LEN, fmt, ap);
    va_end(ap);
}


// is newline when parsing forward: eg ++(tok->at)
static inline bool
is_newline ( Tokenizer *tknzr )
{
    bool result = false;
    char c1 = tokenizer_deref(tknzr);
    char c2 = tokenizer_deref_at(tknzr, 1);
    if ( c1 == '\n' ) {
        // Unix style newline
        result = true;
        ++(tknzr->line_num);
        tknzr->column = -1;
    }
    else if (c1 == '\r' && c2 == '\n') {
        // Windows and MacOS newline
        // Advance the tokenizer by 1 in this case.
        // To pretend the newline made of a higher abstraction
        // thing composed by a single 1 byte
        result = true;
        tokenizer_adv(tknzr);
        ++(tknzr->line_num);
        tknzr->column = 0;
    }
    return result;
}

static inline bool
is_whitespace ( Tokenizer *tknzr )
{
    char c = tokenizer_deref(tknzr);
    return ( is_newline(tknzr)
             || c == '\t'
             || c == '\n'
             || c == '\v'
             || c == '\f'
             || c == '\r'
             || c == ' ' );
}



static inline bool
is_digit ( Tokenizer *tknzr )
{
    char c0 = tokenizer_deref(tknzr);
    char c1 = tokenizer_deref_at(tknzr, 1);
    bool result = false;
    if ( (c0 >= '0' && c0 <= '9')
         || ( c0 == '.' && (c1 >= '0' && c1 <= '9'))) {
        result = true;
    }
    return result;
}

static inline bool
is_alpha ( char c )
{
    return ( (c >= 'A' && c <= 'Z')
             || (c >= 'a' && c <= 'z')
             || (c == '_')
        );
}



// Handles vary newlines encodings from different system
// always returning the newline object
static inline char
preceded_by ( Tokenizer *tknzr )
{
    char result = 0;
    char c  = tokenizer_deref(tknzr);
    char c1 = tokenizer_deref_at(tknzr, -1);
    char c2 = tokenizer_deref_at(tknzr, -2);
    // Handles CR+LF microsoft
    // Considers CR+LF as a single object made by LF
    if ( c == '\n' ) {
        if ( c1 == '\r' ) {
            result = c2;
        }
        else {
            result = c1;
        }
    } else if ( c1 == '\r' ) {
        result = '\n';
    } else {
        result = c1;
    }
    assert (result);
    return result;
}


static inline bool
is_preceded_by ( Tokenizer *tknzr, char c )
{
    return preceded_by(tknzr) == c;
}

// requirements the tokenizer must be at
// the exact start of the comment to eat the comment
// no whitespaces before the start of the comment
// otherwise the function will do nothing
static void
eat_comment ( Tokenizer *tknzr )
{
    // NOTE clears the comment to whitespaces to allow
    // Easier fetching back of the cache so we don't
    // accidently end up in the middle of a comment

    // Maybe change the cache to be smarter and follow the comments
    // in a backward manner?
    if ( tokenizer_deref(tknzr) == '/' && tokenizer_deref_at(tknzr, 1) == '/' ) {
        while ( ! tokenizer_is_end ( tknzr )) {
            if ( is_newline(tknzr)
                 && !is_preceded_by ( tknzr, '\\')) {
                tokenizer_adv ( tknzr );
                break;
            }
            else {
                tokenizer_adv ( tknzr );
            }
        }
    }
    else if ( tokenizer_deref(tknzr) == '/' && tokenizer_deref_at(tknzr, 1) == '*' ) {
        while ( ! tokenizer_is_end ( tknzr )) {
            if ( tokenizer_deref(tknzr) == '*' && (tokenizer_deref_at(tknzr, 1) == '/')) {
                tokenizer_adv ( tknzr );
                tokenizer_adv ( tknzr );
                break;
            }
            else {
                tokenizer_adv ( tknzr );
            }
        }
    }
}


static inline bool
is_punctuator (char c)
{
    return ( (c >= 0x21 && c <= 0x2F)
             || (c >= 0x3A && c <= 0x40)
             || (c >= 0x5B && c <= 0x5E)
             || (c >= 0x7B)
             || (c ==  0x60)  /* ` backtick apostrophe */
             || (c == '#' )
             || ( c == '$')
        ); 
}



static bool
is_start_of_comment (Tokenizer *tknzr)
{
    char c0 = tokenizer_deref(tknzr);
    char c1 = tokenizer_deref_at(tknzr, 1);
    return ( (c0 == '/' && c1 == '/')
             || (c0 == '/' && c1 == '*' ));
}


static void
eat_whitespaces ( Tokenizer *tknzr )
{
    bool done = false;
    while ( !done && !tokenizer_is_end(tknzr) ) {
        if (is_start_of_comment(tknzr)) {
            eat_comment(tknzr);
        }
        else if (is_whitespace(tknzr)) {
            tokenizer_adv(tknzr);
        }
        else {
            break;
        }
    }
    eat_comment(tknzr);

}


static char *keywords[] = {
    "in",
};


static void
parse_identifier_or_keyword ( Tokenizer *tknzr,
                              Token* token )
{
    bool done = false;
    int counter = 0;
    for (; !tokenizer_is_end(tknzr); ++counter ) {
        if (is_punctuator(tokenizer_deref(tknzr))) {
            break;
        }
        else if (is_whitespace(tknzr)) {
            tokenizer_adv(tknzr);
            break;
        }
        if ( is_punctuator(tokenizer_deref(tknzr))
             || is_whitespace(tknzr)) {
            break;
        }
        else {
            tokenizer_adv( tknzr );
        }
    }
    token->type = TT_IDENTIFIER;
    token->text_len = counter;
    u32 i = 0;
    
    for ( i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i ) {
        if ( strncmp (keywords[i], token->text, token->text_len) == 0 ) {
            token->type = TT_KEYWORD;
        }
    }
}


static inline bool
is_start_of_char_const_or_string_literal ( Tokenizer *tknzr )
{
    bool result = false;
    char c1 = tokenizer_deref_at(tknzr, 0);
    char c2 = tokenizer_deref_at(tknzr, 1);
    char c3 = tokenizer_deref_at(tknzr, 2);

    if ( c1 == '\'' || c1 == '\"') {
        result = true;
    }
    else if ( c1 == 'u' && c2 == '8' && c3 == '\"') {
        result = true;
    }
    else if (c1 == 'L' || c1 == 'U' || c1 == 'u') {
        if ( c2 == '\'' || c2 == '\"') {
            result = true;
        }
    }
    return result;

}

static uint32_t
tokenizer_adv_over_start_of_string ( Tokenizer *tknzr, char* punct )
{
    uint32_t result = 0;
    char c1 = tokenizer_deref(tknzr);
    char c2 = tokenizer_deref_at(tknzr, 1);
    char c3 = tokenizer_deref_at(tknzr, 2);
    char c4 = tokenizer_deref_at(tknzr, 3);
    if ( c1 == '\'' || c1 == '\"') {
        result = 1;
        tokenizer_adv_by ( tknzr, result );
        *punct = c1;
    }
    else if ( c1 == 'u' && c2 == '8' && c3 == '\"') {
        result = 3;
        tokenizer_adv_by ( tknzr, result );
        *punct = c3;
    }
    else if (c1 == 'L' || c1 == 'U' || c1 == 'u') {
        if ( c2 == '\'' || c2 == '\"') {
            result = 2;
            tokenizer_adv_by(tknzr, result);
            *punct = c2;
        }
        else {
            invalid_code_path("We should'even be parsing the string, because is_start_of_sring in this case doesn't even fire");
        }
    }

    assert (*punct == '\'' || *punct == '\"');
    assert ( result != 0 );
    return result;
}


static void
parse_digit ( Tokenizer *tknzr,
              Token * token )
{
    int decimal_counter = 0;
    int exponent_counter = 0;
    token->type = TT_CONSTANT;
    int counter = 0;
    for (; !tokenizer_is_end (tknzr); ++counter ) {
        if ( ((tokenizer_deref(tknzr)) == '.') && (decimal_counter == 0)) {
            decimal_counter = 1;
            tokenizer_adv(tknzr);
        }
        else if ( is_punctuator(tokenizer_deref(tknzr))
                  || is_whitespace(tknzr)) {
            char c = tokenizer_deref(tknzr);
               if ( c == '+' || c == '-') {
                    char prev_c = preceded_by(tknzr);
                    if ( (exponent_counter == 0)
                         && (prev_c == 'e' || prev_c == 'E' || prev_c == 'p' || prev_c == 'P')) {
                         ++exponent_counter;
                         tokenizer_adv(tknzr);
                    }
                    else {
                         break;
                    }
               }
               else {
                    break;
               }
          }
          else {
               tokenizer_adv(tknzr);
          }
     }
     token->text_len = counter;
}


static void
parse_punctuator ( Tokenizer *tknzr,
                   Token *token )
{
     // TODO: Add support for   <:   :>   <%   %>   %:   %:%:
     assert ( is_punctuator(tokenizer_deref(tknzr)) );
     token->type = TT_NONE;
     token->text_len = 1;
     int counter = 0;
     // NOTE: this is safe because the content of the file is always zero terminated with 10 bytes
     char c1 = tokenizer_deref(tknzr);
     char c2 = tokenizer_deref_at(tknzr, 1);
     char c3 = tokenizer_deref_at(tknzr, 2);

     if ( c1 == '(') {
          token->type = TT_PUNCT_OPEN_PAREN;
          token->text_len = 1;
     }
     else if ( c1 == ')') {
          token->type = TT_PUNCT_CLOSE_PAREN;
          token->text_len = 1;
     }
     else if ( c1 == '[') {
          token->type = TT_PUNCT_OPEN_BRACKET;
          token->text_len = 1;
     }
     else if ( c1 == ']') {
          token->type = TT_PUNCT_CLOSE_BRACKET;
          token->text_len = 1;
     }
     else if ( c1 == '{') {
          token->type = TT_PUNCT_OPEN_BRACE;
          token->text_len = 1;
     }
     else if ( c1 == '}') {
          token->type = TT_PUNCT_CLOSE_BRACE;
          token->text_len = 1;
     }
     else if ( c1 == ';') {
          token->type = TT_PUNCT_SEMICOLON;
          token->text_len = 1;
     }
     else if ( c1 == '?') {
          token->type = TT_PUNCT_QUESTION_MARK;
          token->text_len = 1;
     }
     else if ( c1 == ':') {
         if ( c2 == '-' ) {
             token->type = TT_PUNCT_DEFINITION;
             token->text_len = 2;

         }
         else {
             token->type = TT_PUNCT_COLON;
             token->text_len = 1;
         }
     }
     else if ( c1 == '.') {
          if ( c2 == '.' && c3 == '.' ) {
               token->type = TT_PUNCT_DOT_DOT_DOT;
               token->text_len = 3;
          }
          else {
               token->type = TT_PUNCT_DOT;
               token->text_len = 1;
          }
     }
     else if ( c1 == ',') {
          token->type = TT_PUNCT_COMMA;
          token->text_len = 1;
     }
     else if ( c1 == '\\') {
          token->type = TT_PUNCT_BACKWARD_SLASH;
          token->text_len = 1;
     }
     else if ( c1 == '`') {
          token->type = TT_PUNCT_BACKWARD_APOSTROPHE;
          token->text_len = 1;
     }
     else if (c1 == '$') {
          token->type = TT_PUNCT_DOLLAR_SIGN;
          token->text_len = 1;
     }
     else if (c1 == '@') {
          token->type = TT_PUNCT_AT_SIGN;
          token->text_len = 1;
     }
     else if ( c1 == '#' ) {
         token->type = TT_PUNCT_POUND;
         token->text_len = 1;
     }
     else if ( c1 == '+') {
          if ( c2 == '+') {
               token->type = TT_PUNCT_INCREMENT;
               token->text_len = 2;
          }
          else if ( c2 == '=' ) {
               token->type = TT_PUNCT_ADDITION_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_PLUS;
               token->text_len = 1;
          }
     }
     else if ( c1 == '-') {
          if ( c2 == '-' ) {
               token->type = TT_PUNCT_DECREMENT;
               token->text_len = 2;
          }
          else if ( c2 == '>' ) {
               token->type = TT_PUNCT_ARROW;
               token->text_len = 2;
          }
          else if ( c2 == '=' ) {
               token->type = TT_PUNCT_SUBTRACTION_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_MINUS;
               token->text_len = 1;
          }
     }
     else if ( c1 == '*' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_MULTIPLICATION_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_ASTERISK;
               token->text_len = 1;
          }
     }
     else if (c1 == '/' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_DIVISION_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_DIVISION;
               token->text_len = 1;
          }
     }
     else if (c1 == '%' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_MODULO_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_MODULO;
               token->text_len = 1;
          }

     }
     else if ( c1 == '=' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_EQUAL_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_EQUAL;
               token->text_len = 1;
          }
     }
     else if ( c1 == '!' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_NOT_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_LOGICAL_NOT;
               token->text_len = 1;
          }
     }
     else if ( c1 == '>' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_GREATER_OR_EQUAL;
               token->text_len = 2;
          }
          else if ( c2 == '>') {
               if ( c3 == '=' ) {
                    token->type = TT_PUNCT_BITWISE_RIGHT_SHIFT_EQUAL;
                    token->text_len = 3;
               }
               else {
                    token->type = TT_PUNCT_BITWISE_RIGHT_SHIFT;
                    token->text_len = 2;
               }
          }
          else {
               token->type = TT_PUNCT_GREATER;
               token->text_len = 1;
          }
     }
     else if ( c1 == '<' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_LESS_OR_EQUAL;
               token->text_len = 2;
          }
          else if ( c2 == '<') {
               if ( c3 == '=' ) {
                    token->type = TT_PUNCT_BITWISE_LEFT_SHIFT_EQUAL;
                    token->text_len = 3;
               }
               else {
                    token->type = TT_PUNCT_BITWISE_LEFT_SHIFT;
                    token->text_len = 2;
               }
          }
          else if (c2 == '-' && c3 == '>') {
              token->type = TT_PUNCT_BOTHDIR_ARROW;
              token->text_len = 3;
          }
          else {
               token->type = TT_PUNCT_LESS;
               token->text_len = 1;
          }
     }
     else if ( c1 == '~' ) {
          token->type = TT_PUNCT_BITWISE_NOT;
          token->text_len = 1;
     }
     else if ( c1 == '&' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_BITWISE_AND_EQUAL;
               token->text_len = 2;
          }
          else if (c2 == '&') {
               token->type = TT_PUNCT_LOGICAL_AND;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_BITWISE_AND;
               token->text_len = 1;
          }
     }
     else if ( c1 == '|' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_BITWISE_OR_EQUAL;
               token->text_len = 2;
          }
          else if (c2 == '|') {
               token->type = TT_PUNCT_LOGICAL_OR;
               token->text_len = 2;
          } else if (c2 == '-' || c2 == '=' ) {
              token->type = TT_PUNCT_DEDUCTION;
               token->text_len = 2;

          }
          else {
               token->type = TT_PUNCT_BITWISE_OR;
               token->text_len = 1;
          }
     }
     else if ( c1 == '^' ) {
          if ( c2 == '=' ) {
               token->type = TT_PUNCT_BITWISE_XOR_EQUAL;
               token->text_len = 2;
          }
          else {
               token->type = TT_PUNCT_BITWISE_XOR;
               token->text_len = 1;
          }
     }
     assert (token->type != TT_NONE);
     tokenizer_adv_by ( tknzr, token->text_len );
}


static inline void
tokenizer_clear_error ( Tokenizer *tknzr )
{
     tknzr->err = 0;
     tknzr->err_desc[0] = 0;
}


bool
get_next_token ( Tokenizer *tknzr,
                 Token *token )
{

     Token local_token_for_null;

     token = token == NULL ? &local_token_for_null : token;

     i32 next_token_idx = -1;
     {

          if ( tokenizer_is_end(tknzr)) {
               return false;
          }
          tokenizer_clear_error (tknzr);
          // TODO: Handle splitting the tokens for preprocessing directive instead of giving the entire line
          eat_whitespaces(tknzr);
          token->text = tknzr->at;
          token->line_num = tknzr->line_num;
          token->column = tknzr->column;
          if ( is_digit( tknzr ) ) {
               parse_digit ( tknzr, token );
          } else if ( is_punctuator (tokenizer_deref(tknzr) )) {
               parse_punctuator(tknzr, token);
          } else if ( is_alpha (tokenizer_deref(tknzr)) || tokenizer_deref(tknzr) == '_' ) {
               parse_identifier_or_keyword ( tknzr, token );
          } else {
              token->type = TT_NONE;
              token->text = 0;
              token->text_len = 0;
              return false;
          }
          eat_whitespaces(tknzr);
     }
     return true;
}





bool
discard_next_token ( Tokenizer* tok )
{
     return get_next_token ( tok, NULL );
}

void
tokenizer_init_from_memory( Tokenizer *tknzr,
                            char *buffer, size_t buffersize,
                            char *filename)
{
    (void) tknzr;
    assert(buffer);
    assert(buffersize);
    tknzr->base = buffer;
    tknzr->base_len = buffersize;
    strncpy (tknzr->file, filename, TOKENIZER_FILENAME_LEN);
    tknzr->line_num = 1;
    tknzr->column = 0;
    tknzr->at = tknzr->base;

    tknzr->err = 0;
    tknzr->err_desc[0] = 0;
}


void
tokenizer_init_with_memmapped_file (Tokenizer *tknzr, char* filename )
{
    const i32 ZEROED_CHUNK_SIZE = 8;
    i64 file_len = 0;
    void *buffer = alloc_memmapped_file(filename, 0x000, PAGE_PROT_READ | PAGE_PROT_WRITE,
                                        PAGE_ANONYMOUS | PAGE_PRIVATE, true, ZEROED_CHUNK_SIZE,
                                        &file_len);
    assert(buffer);
    assert(file_len);
    tokenizer_init_from_memory(tknzr, buffer, file_len, filename);
}


// TODO: Remove me
void
tokenizer_init(Tokenizer *tknzr, char *filename)
{
     tokenizer_init_with_memmapped_file(tknzr, filename);
}


bool
compare_token_to ( Token * token, char *token_text )
{
     bool result = false;
     if ( 0 == strncmp(token_text, token->text, token->text_len ) ) {
          result = true;
     }
     return result;
}


bool
compare_next_token_to (Tokenizer *tknzr, char * token_text )
{
     Token token;
     bool result = false;
     if ( get_next_token(tknzr, &token) ) {
          if (compare_token_to (&token, token_text)) {
               result = true;
          }
     }
     return result;
}




void log_token_text (FILE *stream, Token *token)
{
    char *text = token->text;
    int text_len = token->text_len;
    fprintf(stream, "%.*s", text_len, text);
}




void log_token (Token *token)
{
    log_token_text(stdin, token);
    fprintf(stdout, "\tline_number=%d\tcolumn=%d\n", token->line_num, token->column);
}



#endif /* PARSER_C_IMPL */
