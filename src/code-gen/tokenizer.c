#ifndef TOKENIZER_C_INCLUDE
#define TOKENIZER_C_INCLUDE

#include "utils.c"
#include "platform.c"


// TODO: Remove these dependencies and make use of the hardware abstraction layer
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>

enum Token_Type {
     TT_NONE = 0,
     TT_COMMENT = 1,
     TT_WHITESPACES = 2,
     TT_IDENTIFIER = 3,
     TT_PUNCT_YET_TO_COMPUTE = 4,

     TT_META = 5,
     
     TT_STRING_LITERAL,
     TT_CONSTANT,
     TT_PP_DIRECTIVE,

     // -----------------------
     // VERY IMPORTANT: Order does matter: it should align with the order
     // of the c11 keywords inside the tokenizer.c
     // Good note: crap like this is fixed in languages like Nim
     // ------------------------
     TT_KEYWORD_alignof,
     TT_KEYWORD_auto,
     TT_KEYWORD_break,
     TT_KEYWORD_case,
     TT_KEYWORD_char,
     TT_KEYWORD_const,
     TT_KEYWORD_continue,
     TT_KEYWORD_default,

     TT_KEYWORD_do,
     TT_KEYWORD_double,
     TT_KEYWORD_else,
     TT_KEYWORD_enum,
     TT_KEYWORD_extern,
     TT_KEYWORD_float,
     TT_KEYWORD_for,
     TT_KEYWORD_goto,
     TT_KEYWORD_if,

     TT_KEYWORD_inline,
     TT_KEYWORD_int,
     TT_KEYWORD_long,
     TT_KEYWORD_register,
     TT_KEYWORD_restrict,
     TT_KEYWORD_return,
     TT_KEYWORD_short,
     TT_KEYWORD_signed,

     TT_KEYWORD_sizeof,
     TT_KEYWORD_static,
     TT_KEYWORD_struct,
     TT_KEYWORD_switch,
     TT_KEYWORD_typedef,
     TT_KEYWORD_union,
     TT_KEYWORD_unsigned,
     TT_KEYWORD_void,
     // --------------------------------

     TT_KEYWORD_volatile,
     TT_KEYWORD_while,
     TT_KEYWORD_Alignas,
     TT_KEYWORD_Atomic,
     TT_KEYWORD_Bool,
     TT_KEYWORD_Complex,
     TT_KEYWORD_Generic,

     TT_KEYWORD_Imaginary,
     TT_KEYWORD_Noreturn,
     TT_KEYWORD_Static_assert,
     TT_KEYWORD_Thread_local,

     TT_PUNCT_BACKWARD_SLASH,
     TT_PUNCT_BACKWARD_APOSTROPHE,
     TT_PUNCT_AT_SIGN,


     TT_PUNCT_EQUAL,
     TT_PUNCT_PLUS,
     TT_PUNCT_MINUS,
     TT_PUNCT_ASTERISK,
     TT_PUNCT_DIVISION,
     TT_PUNCT_MODULO,
     TT_PUNCT_INCREMENT,
     TT_PUNCT_DECREMENT,

     TT_PUNCT_EQUAL_EQUAL,
     TT_PUNCT_NOT_EQUAL,
     TT_PUNCT_GREATER,
     TT_PUNCT_LESS,
     TT_PUNCT_GREATER_OR_EQUAL,
     TT_PUNCT_LESS_OR_EQUAL,


     TT_PUNCT_LOGICAL_NOT,
     TT_PUNCT_LOGICAL_AND,
     TT_PUNCT_LOGICAL_OR,

     TT_PUNCT_BITWISE_NOT,
     TT_PUNCT_BITWISE_AND,
     TT_PUNCT_BITWISE_OR,
     TT_PUNCT_BITWISE_XOR,
     TT_PUNCT_BITWISE_LEFT_SHIFT,
     TT_PUNCT_BITWISE_RIGHT_SHIFT,

     TT_PUNCT_ADDITION_EQUAL,
     TT_PUNCT_SUBTRACTION_EQUAL,
     TT_PUNCT_MULTIPLICATION_EQUAL,
     TT_PUNCT_DIVISION_EQUAL,
     TT_PUNCT_MODULO_EQUAL,
     TT_PUNCT_BITWISE_AND_EQUAL,
     TT_PUNCT_BITWISE_OR_EQUAL,
     TT_PUNCT_BITWISE_XOR_EQUAL,
     TT_PUNCT_BITWISE_LEFT_SHIFT_EQUAL,
     TT_PUNCT_BITWISE_RIGHT_SHIFT_EQUAL,


     TT_PUNCT_DOT_DOT_DOT,

     TT_PUNCT_DOT,
     TT_PUNCT_COMMA,
     TT_PUNCT_SEMICOLON, // ;
     TT_PUNCT_COLON,     // :
     TT_PUNCT_ARROW,

     TT_PUNCT_OPEN_PAREN,
     TT_PUNCT_CLOSE_PAREN,
     TT_PUNCT_OPEN_BRACKET,
     TT_PUNCT_CLOSE_BRACKET,
     TT_PUNCT_OPEN_BRACE,
     TT_PUNCT_CLOSE_BRACE,


     TT_PUNCT_QUESTION_MARK,

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


// Caching mecahnism for recently parsed tokens
// Circular buffer implementation base

typedef struct Tokenizer {
     char file[TOKENIZER_FILENAME_LEN];

     i32 line_num;

     char *base; // Must point to content of file + at least 4 bytes of 0 termination
                 // and preceded by at least 4 bytes of 0x00
     i64 base_len;

     enum Tokenization_Error err;
     char err_desc[TOKENIZER_ERR_DESC_LEN];

     char *at;
} Tokenizer;




void
tokenizer_init (Tokenizer* tok, char* filename );


void
tokenizer_push_state (Tokenizer *tknzr);

bool
tokenizer_pop_state (Tokenizer *tknzr);


bool
get_prev_token ( Tokenizer *tknzr, Token *token );


bool
get_next_token ( Tokenizer *tknzr,
                 Token *token );

bool
discard_next_token ( Tokenizer* tok );


bool
peek_prev_token ( Tokenizer *tknzr, Token *token );


bool
peek_next_token ( Tokenizer *tknzr,
                  Token *token );


bool
compare_token_to ( Token *token, char *token_text );

bool
compare_next_token_to (Tokenizer *tknzr, char * token_text );

// #######################################################################
#endif /* TOKENIZER_C_INCLUDE */
#if !defined TOKENIZER_C_IMPLEMENTED && defined TOKENIZER_C_IMPL
#define TOKENIZER_C_IMPLEMENTED
// #######################################################################


#include <stdarg.h>
#include <stddef.h>
#include <memory.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int strcmp(const char *s1, const char *s2);


static inline char
tokenizer_deref_at( Tokenizer *tknzr, int rel )
{
    if ( (((uint8_t *)tknzr->at + rel) >= (uint8_t*)tknzr->base)
         && ((uint8_t *)tknzr->at + rel) < ((uint8_t *)tknzr->base + tknzr->base_len) ) {
        return *(tknzr->at + rel);
    } else {
        return 0;
    }
}

static inline char
tokenizer_deref(Tokenizer *tknzr)
{
    return tokenizer_deref_at(tknzr, 0);
}

static bool
tokenizer_is_end ( Tokenizer *tknzr )
{
    bool result = false;
    char c = tokenizer_deref(tknzr);
    if ( c == '\0' || c ==  0x04) {
        result = true;
    }
    return result;
}



static inline void
tokenizer_adv_by ( Tokenizer *tknzr, uint32_t step )
{
     (tknzr->at) += step;
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
     char prev = tokenizer_deref_at(tknzr, -1);
     char c1 = tokenizer_deref(tknzr);
     char c2 = tokenizer_deref_at(tknzr, 1);
     if ( c1 == '\n' || c1 == '\r') {
          // Unix style newline
          result = true;
          ++(tknzr->line_num);
     } else {
         result = false;
     }
     return result;
}

static inline bool
is_whitespace ( Tokenizer *tknzr )
{
    char c = tokenizer_deref(tknzr);
    bool result = is_newline(tknzr)
        || c == '\t'
        || c == '\n'
        || c == '\v'
        || c == '\f'
        || c == '\r'
        || c == ' ';
    return (result);
}



static inline bool
is_digit ( Tokenizer *tknzr )
{
    char c0 = tokenizer_deref_at(tknzr, 0);
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
#if TOKENIZER_DOLLAR_SIGN_VALID_IDENTIFIER
     if ( c == '$' ) {
          return true;
     }
#endif
     return ( (c >= 'A' && c <= 'Z')
              || (c >= 'a' && c <= 'z')
              || (c == '_'));
}



// Handles vary newlines encodings from different system
// always returning the newline object
static inline char
preceded_by ( Tokenizer *tknzr )
{
     // NOTE: This always work because we allocate
     // a zeroed out page at the start of file mapping
     char result = -1;
     char c  = tokenizer_deref_at(tknzr, 0);
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
     }
     else if ( c1 == '\r' ) {
          result = '\n';
     }
     else {
          result = c1;
     }
     assert (result);
     return result;
}


static inline bool
is_preceded_by_char ( Tokenizer *tknzr, char c )
{
     return preceded_by(tknzr) == c;
}

// requirements the tokenizer must be at
// the exact start of the comment to eat the comment
// no whitespaces before the start of the comment
// otherwise the function will do nothing
static size_t
eat_comment ( Tokenizer *tknzr )
{
    size_t len = 0;
    if ( tokenizer_deref(tknzr) == '/' && tokenizer_deref_at(tknzr, 1) == '/' ) {
        while ( ! tokenizer_is_end ( tknzr )) {
            if ( is_newline(tknzr)
                 && !is_preceded_by_char ( tknzr, '\\')) {
                tokenizer_adv ( tknzr );
                len ++;
                break;
            }
            else {
                tokenizer_adv ( tknzr );
            }
            len++;
        }
     }
    else if ( tokenizer_deref(tknzr) == '/' && tokenizer_deref_at(tknzr, 1) == '*' ) {
        while ( ! tokenizer_is_end ( tknzr )) {
            if ( tokenizer_deref(tknzr) == '*' && (tokenizer_deref_at(tknzr, 1) == '/')) {
                tokenizer_adv ( tknzr );
                tokenizer_adv ( tknzr );
                len += 2;
                break;
            }
            else {
                tokenizer_adv ( tknzr );
            }
            len++;
        }
     }
    return len;
 }


static inline bool
is_punctuator (char c)
{
     // NOTE: See ascii table
#if TOKENIZER_DOLLAR_SIGN_VALID_IDENTIFIER
     if ( c == '$' ) {
          return false;
     }
#endif
     return ( (c >= 0x21 && c <= 0x2F)
              || (c >= 0x3A && c <= 0x40)
              || (c >= 0x5B && c <= 0x5E)
              || (c >= 0x7B) );
}

static inline bool
is_meta(Tokenizer *tknzr)
{
    char c = tokenizer_deref(tknzr);
    if ( c == '$') {
        return true;
    } else {
        return false;
    }
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
eat_whitespaces ( Tokenizer *tknzr)
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


static void
parse_whitespaces ( Tokenizer *tknzr,
                    Token *token )
{
    assert(is_whitespace(tknzr));
    bool done = false;
    size_t counter = 0;
    token->type = TT_WHITESPACES;
    token->text = tknzr->at;
    
    while ( is_whitespace(tknzr) && !tokenizer_is_end(tknzr)) {
        tokenizer_adv(tknzr);
        counter++;
    }
    token->text_len = counter;
}


static void
parse_comment ( Tokenizer *tknzr,
                Token *token )
{
    assert( is_start_of_comment(tknzr));
    bool done = false;
    size_t counter;
    token->type = TT_COMMENT;
    token->text = tknzr->at;

    token->text_len = eat_comment(tknzr);
}



// -----------------------
// VERY IMPORTANT: Order does matter: it should align with the order
// of the enum defined in tokenizer.h
// Good note: crap like this is fixed in languages like Nim
// ------------------------
static char *c11_keywords[] = {
     "alignof", "auto", "break", "case", "char", "const", "continue", "default",
     "do", "double", "else", "enum", "extern", "float", "for", "goto", "if",
     "inline", "int", "long", "register", "restrict", "return", "short", "signed",
     "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
     "volatile", "while", "_Alignas", "_Atomic", "_Bool", "_Complex", "_Generic",
     "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local",
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
               break;
          }
          else {
               tokenizer_adv( tknzr );
          }
     }
     token->type = TT_IDENTIFIER;
     token->text_len = counter;
     u32 i = 0;
     for ( i = 0; i < sizeof(c11_keywords) / sizeof(c11_keywords[0]); ++i ) {
          if ( strncmp (c11_keywords[i], token->text, token->text_len) == 0 ) {
               token->type = TT_KEYWORD_alignof + i;
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
     char c1 = tokenizer_deref_at(tknzr, 0);
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
parse_char_const_or_string_literal ( Tokenizer *tknzr,
                                     Token *token )
{
     char str_end_punct;
     token->text_len = 0;
     (token->text_len) += tokenizer_adv_over_start_of_string( tknzr, &str_end_punct );
     assert ( str_end_punct == '\'' || str_end_punct == '\"');
     if ( str_end_punct == '\"') {
          token->type = TT_STRING_LITERAL;
     }
     else {
          token->type = TT_CONSTANT;
     }
     int counter = 0;
     bool is_prev_backslash = false;
     for (; !tokenizer_is_end(tknzr); ++counter ) {
          if ( tokenizer_deref(tknzr) == '\\' ) {
               if (is_prev_backslash ) {
                    is_prev_backslash = false;
               }
               else {
                    is_prev_backslash = true;
               }
               tokenizer_adv(tknzr);
          }
          else if ( is_newline(tknzr)) {
               if (!is_preceded_by_char ( tknzr, '\\')) {
                    tokenizer_err_fmt (tknzr, TOK_ERR_INVALID_STRING,
                                       "%s: Invalid use of new line while parsing string at line: %d ", tknzr->file, tknzr->line_num);
                    token->text_len = 0;
                    return;
               }
               else {
                    tokenizer_adv (tknzr);
               }
               is_prev_backslash = false;
          }
          else if ( tokenizer_deref(tknzr) == str_end_punct
                    && is_prev_backslash == false) {
               if (is_prev_backslash == false ) {
                    break;
               }
               else {
                    tokenizer_adv(tknzr);
                    is_prev_backslash = false;
               }
          }
          else {
               tokenizer_adv(tknzr);
               is_prev_backslash = false;
          }
     }
     token->text_len += counter + 1;
     tokenizer_adv (tknzr);
}


// TODO: Eats the entire preprocessor directive line, maybe we want to provide an extra
// step to split it into tokens ?
static void
parse_preprocessor_directive( Tokenizer *tknzr,
                              Token *token )
{
     int counter = 0;
     // Preconditions for calling the function
     assert ( tokenizer_deref(tknzr) == '#' );
     token->type = TT_PP_DIRECTIVE;

     for (counter = 0; !tokenizer_is_end (tknzr); ++counter ) {
          if (is_start_of_comment(tknzr)) {
               eat_comment(tknzr);
          }
          if ( is_newline(tknzr) ) {
               if ( !is_preceded_by_char(tknzr, '\\')) {
                    tokenizer_adv(tknzr);
                    break;
               }
          }
          tokenizer_adv(tknzr);
     }

     token->text_len = counter;
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
parse_meta ( Tokenizer *tknzr,
             Token *token )
{
    bool done = false;
    int counter = 0;

    int num_parens = 0;
    bool followed_by_paren = false;
    char c = tokenizer_deref(tknzr);
    assert(c == '$');
    c = tokenizer_deref_at(tknzr, +1);
    if ( c == '(')  {
        followed_by_paren = true;
        tokenizer_adv_by(tknzr, 2);
        token->text = tknzr->at;
        num_parens = 1;
    } else {
        followed_by_paren = false;
        tokenizer_adv_by(tknzr, 1);
        token->text = tknzr->at;
        num_parens = 0;
    }
    
    
    for (; !tokenizer_is_end(tknzr); ++counter ) {
        char c = tokenizer_deref(tknzr);
        if (is_punctuator(c)) {
            if ( !followed_by_paren ){
                break;
            } else {
                if ( c == '(') {
                    num_parens ++;
                } else if ( c == ')') {
                    if (num_parens == 0 ) {
                        tokenizer_adv(tknzr);
                        counter--;
                        break;
                    } else
                        num_parens--;
                } else {
                    break;
                }
            }
        } else if (is_whitespace(tknzr)) {
            break;
        } else {
            tokenizer_adv( tknzr );
        }
    }
    token->type = TT_META;
    token->text_len = counter;
}

static void
parse_punctuator ( Tokenizer *tknzr,
                   Token *token )
{
    // TODO: Add support for   <:   :>   <%   %>   %:   %:%:
    assert ( is_punctuator(tokenizer_deref(tknzr)) );
    token->type = TT_PUNCT_YET_TO_COMPUTE;
    token->text_len = 1;
    int counter = 0;
    // NOTE: this is safe because the content of the file is always zero terminated with 10 bytes
    char c1 = tokenizer_deref_at(tknzr, 0);
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
          token->type = TT_PUNCT_COLON;
          token->text_len = 1;
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
     else if (c1 == '@') {
          token->type = TT_PUNCT_AT_SIGN;
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
     assert (token->type != TT_PUNCT_YET_TO_COMPUTE);
     (tknzr->at) += token->text_len;
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


     if ( tokenizer_is_end(tknzr)) {
         return false;
     }
     tokenizer_clear_error (tknzr);
     // TODO: Handle splitting the tokens for preprocessing directive instead of giving the entire line
     token->text = tknzr->at;
     token->line_num = tknzr->line_num;

     if ( is_whitespace(tknzr)) {
         parse_whitespaces(tknzr, token);
     } else if (is_start_of_comment(tknzr)) {
         parse_comment(tknzr, token);
     } else if ( (* tknzr->at) == '#' ) {
         parse_preprocessor_directive(tknzr, token);
     }
     else if ( is_digit( tknzr ) ) {
         parse_digit ( tknzr, token );
     }
     else if ( is_start_of_char_const_or_string_literal(tknzr)) {
         parse_char_const_or_string_literal (tknzr, token );
     }
     else if (is_meta(tknzr)) {
         parse_meta(tknzr, token);
     } else if ( is_punctuator (tokenizer_deref(tknzr) )) {
         parse_punctuator(tknzr, token);
     }
     else if ( is_alpha (tokenizer_deref(tknzr)) || tokenizer_deref(tknzr) == '_' ) {
         parse_identifier_or_keyword ( tknzr, token );
     }
     return true;
}





bool
discard_next_token ( Tokenizer* tok )
{
     return get_next_token ( tok, NULL );
}




void
tokenizer_init_from_memory(Tokenizer *tknzr,
                           char *buffer, size_t buffer_len)
{
     (void) tknzr;
     assert(buffer);
     assert(buffer_len);
     tknzr->base = buffer;
     tknzr->base_len = buffer_len;
     tknzr->line_num = 1;
     tknzr->at = tknzr->base;

     tknzr->err = 0;
     tknzr->err_desc[0] = 0;

}


void
tokenizer_init_with_memmapped_file (Tokenizer *tknzr, char* filename )
{
    i64 file_len = 0;
    void *buffer = alloc_memmapped_file(filename, 0x000, PAGE_PROT_READ | PAGE_PROT_WRITE,
                                        PAGE_ANONYMOUS | PAGE_PRIVATE, false, 0,
                                        &file_len);
    strncpy (tknzr->file, filename, TOKENIZER_FILENAME_LEN);
    tokenizer_init_from_memory(tknzr, buffer, (size_t) file_len);
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


#endif /* TOKENIZER_C_IMPL */
