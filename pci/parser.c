#ifndef PARSER_C_INCLUDE
#define PARSER_C_INCLUDE

#include "utils.c"
#include "platform.c"


#if defined __DEBUG
#define TOKENIZER_DEBUG 1
#else
#define TOKENIZER_DEBUG 0
#endif

#define TOKENIZER_CACHING_ENABLED 0
#define TOKENIZER_STATE_LIST_ENABLED 0
#define TOKENIZER_DOLLAR_SIGN_VALID_IDENTIFIER (1)

enum Token_Type {
     TT_NONE = 0,
     TT_IDENTIFIER = 1,
     TT_CONSTANT,


     TT_PUNCT_ENUM_OPERATORS_START_MARKER,
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



     
     TT_PUNCT_ENUM_MARKER_NOT_IMPLEMENTED_OPERATORS, // marker
     // enums that follows this marker are parsed but not implemented
     // semantically


     TT_PUNCT_BACKWARD_SLASH,
     TT_PUNCT_BACKWARD_APOSTROPHE,
     TT_PUNCT_AT_SIGN,


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

     TT_PUNCT_COLON,     // :


     TT_PUNCT_ENUM_OPERATORS_END_MARKER,

     TT_PUNCT_OPEN_PAREN,
     TT_PUNCT_CLOSE_PAREN,
     TT_PUNCT_OPEN_BRACKET,
     TT_PUNCT_CLOSE_BRACKET,
     TT_PUNCT_OPEN_BRACE,
     TT_PUNCT_CLOSE_BRACE,


     TT_PUNCT_QUESTION_MARK,
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



// Caching mechanism for recently parsed tokens
// Circular buffer implementation base
typedef struct Tokenizer_Cache {
     Token* buffer;
     i32 buffer_len;

     i32 at;
     i32 start;
     i32 end;
} Tokenizer_Cache;


typedef struct Tokenizer_State {
     char *at;
     i32 line_num;
     i32 column;
} Tokenizer_State;


typedef struct Tokenizer_State_List {
     #define TOKENIZER_MAX_NUM_STATES (PAGE_SIZE - sizeof(struct Tokenizer_State_List*) - sizeof(i32)) / sizeof(struct Tokenizer_State)
     // 4096 is usually a page size
     struct Tokenizer_State_List *next;
     i32 used;
     Tokenizer_State states[TOKENIZER_MAX_NUM_STATES];
} Tokenizer_State_List;


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

#if TOKENIZER_STATE_LIST_ENABLED
     Tokenizer_State_List *list;
#endif
#if TOKENIZER_CACHING_ENABLED
    Tokenizer_Cache cache;
#endif
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
#endif /* PARSER_C_INCLUDE */
#if !defined PARSER_C_IMPLEMENTED && defined PARSER_C_IMPL
#define PARSER_C_IMPLEMENTED
// #######################################################################


#include <stdarg.h>
#include <stddef.h>
#include <memory.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int strcmp(const char *s1, const char *s2);


static bool
tokenizer_is_end ( Tokenizer *tknzr )
{
    bool result = false;
    if ( *tknzr->at == 0 ) {
        result = true;
    }
    return result;
}



static inline void
tokenizer_adv_by ( Tokenizer *tknzr, uint32_t step )
{
     (tknzr->at) += step;
     tknzr->column += step;
     assert ( tknzr->at < tknzr->base + tknzr->base_len );
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
is_newline_fwd ( Tokenizer *tknzr )
{
     bool result = false;
     char prev = *(tknzr->at - 1);
     char c1 = *tknzr->at;
     char c2 = *(tknzr->at + 1);
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
     else if ((prev == '\r') && (c1 == '\n')) {
          assert_msg(0, "Needs testing to see if the line counting mechanism"
                     "works with this type of newline encoding");
          result = true;
          ++(tknzr->line_num);
          tknzr->column = 0;
     }
     return result;
}

// is newline when parsing backword: eg --(tok->at)
static inline bool
is_newline_bwd(Tokenizer *tknzr)
{
     bool result = false;
     char c = *(tknzr->at);
     char prev1 = *(tknzr->at - 1);
     char prev2 = *(tknzr->at - 2);

     if ( c == '\n') {
          result = true;
          --(tknzr->line_num);
          if ( prev1 == '\r') {
               // microsoft encoding CR + LF
               --(tknzr->at);
          }
     }
     else if ( c == '\r' ) {
          result = true;
          --(tknzr->line_num);
     }
     return result;
}

static inline bool
is_whitespace ( Tokenizer *tknzr )
{
     char c = *tknzr->at;
     return ( is_newline_fwd(tknzr)
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
    char c0 = *(tknzr->at);
    char c1 = *(tknzr->at + 1);
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
#        if TOKENIZER_DOLLAR_SIGN_VALID_IDENTIFIER
              || (c== '$')
#        endif
         );
}



// Handles vary newlines encodings from different system
// always returning the newline object
static inline char
preceded_by ( Tokenizer *tknzr )
{
     // NOTE: This always work because we allocate
     // a zeroed out page at the start of file mapping
     char result = -1;
     char c  = *(tknzr->at - 0);
     char c1 = *(tknzr->at - 1);
     char c2 = *(tknzr->at - 2);
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
is_preceded_by_char ( Tokenizer *tknzr, char c )
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
     if ( *tknzr->at == '/' && *(tknzr->at + 1) == '/' ) {
          *tknzr->at = ' ';
          *(tknzr->at + 1) = ' ';
          while ( ! tokenizer_is_end ( tknzr )) {
               if ( is_newline_fwd(tknzr)
                    && !is_preceded_by_char ( tknzr, '\\')) {
                    tokenizer_adv ( tknzr );
                    break;
               }
               else {
                    tokenizer_adv ( tknzr );
               }
          }
     }
     else if ( *tknzr->at == '/' && *(tknzr->at + 1) == '*' ) {
          *tknzr->at = ' ';
          *(tknzr->at + 1) = ' ';
          while ( ! tokenizer_is_end ( tknzr )) {
               if ( *(tknzr->at) == '*' && (*(tknzr->at + 1) == '/')) {
                    *tknzr->at = ' ';
                    *(tknzr->at + 1) = ' ';
                    tokenizer_adv ( tknzr );
                    tokenizer_adv ( tknzr );
                    break;
               }
               else {
                    if (!is_newline_fwd(tknzr)) {
                         *(tknzr->at) = ' ';
                    }
                    tokenizer_adv ( tknzr );
               }
          }
     }
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



static bool
is_start_of_comment (Tokenizer *tknzr)
{
     char c0 = *(tknzr->at);
     char c1 = *(tknzr->at + 1);
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



// -----------------------
// VERY IMPORTANT: Order does matter: it should align with the order
// of the enum defined in tokenizer.h
// Good note: crap like this is fixed in languages like Nim
// ------------------------
static char *keywords[] = {
};






static void
parse_identifier_or_keyword ( Tokenizer *tknzr,
                              Token* token )
{
     bool done = false;
     int counter = 0;
     for (; !tokenizer_is_end(tknzr); ++counter ) {
          if (is_punctuator(*tknzr->at)) {
               break;
          }
          else if (is_whitespace(tknzr)) {
               tokenizer_adv(tknzr);
               break;
          }
          if ( is_punctuator(*tknzr->at)
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
#if 0
     for ( i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i ) {
          if ( strncmp (keywords[i], token->text, token->text_len) == 0 ) {
              assert_msg(0, "For now keywords are not supported");
          }
     }
#endif
}


static inline bool
is_start_of_char_const_or_string_literal ( Tokenizer *tknzr )
{
     bool result = false;
     char c1 = *(tknzr->at + 0);
     char c2 = *(tknzr->at + 1);
     char c3 = *(tknzr->at + 2);

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
     char c1 = *(tknzr->at + 0);
     char c2 = *(tknzr->at + 1);
     char c3 = *(tknzr->at + 2);
     char c4 = *(tknzr->at + 3);
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


// TODO: Eats the entire preprocessor directive line, maybe we want to provide an extra
// step to split it into tokens ?

static void
parse_digit ( Tokenizer *tknzr,
              Token * token )
{
     int decimal_counter = 0;
     int exponent_counter = 0;
     token->type = TT_CONSTANT;
     int counter = 0;
     for (; !tokenizer_is_end (tknzr); ++counter ) {
          if ( ((*tknzr->at) == '.') && (decimal_counter == 0)) {
               decimal_counter = 1;
               tokenizer_adv(tknzr);
          }
          else if ( is_punctuator(*tknzr->at)
                    || is_whitespace(tknzr)) {
               char c = *tknzr->at;
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
     assert ( is_punctuator(*tknzr->at) );
     token->type = TT_NONE;
     token->text_len = 1;
     int counter = 0;
     // NOTE: this is safe because the content of the file is always zero terminated with 10 bytes
     char c1 = *(tknzr->at + 0);
     char c2 = *(tknzr->at + 1);
     char c3 = *(tknzr->at + 2);

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
#if !TOKENIZER_DOLLAR_SIGN_VALID_IDENTIFIER
     else if (c1 == '$') {
          token->type = TT_PUNCT_DOLLAR_SIGN;
          token->text_len = 1;
     }
#endif
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
     (tknzr->column) += token->text_len;
     (tknzr->at) += token->text_len;
}


static inline void
tokenizer_clear_error ( Tokenizer *tknzr )
{
     tknzr->err = 0;
     tknzr->err_desc[0] = 0;
}


static inline void
cache_inc_index (Tokenizer_Cache *cache, i32 *index )
{
     *index = (*index + 1) % cache->buffer_len;
}



static inline void
cache_dec_index (Tokenizer_Cache *cache, i32 *index )
{
     *index = (*index - 1) % cache->buffer_len;
}

static inline void
cache_clamp_index (Tokenizer_Cache *cache, i32 *index )
{
     *index = (*index) % cache->buffer_len;
}



// Finds and returns the index where the search is
static i32
__debug_cache_find_bin_search ( Tokenizer_Cache *cache,
                                char *search )
{
     for (i32 i = cache->start; i != cache->end; cache_inc_index(cache, &i)) {
          if ( cache->buffer[i].text == search ){
               return i;
          }
     }
     return -1;
}


static i32
cache_find_bin_search ( Tokenizer_Cache *cache,
                        char *search )
{
     i32 result = -1;
     i32 l = 0;
     i32 r = -1; {
          if ( cache->end >= cache->start ) {
               r = cache->end - cache->start;
          }
          else {
               i32 diff = cache->start - cache->end;
               r = cache->buffer_len - diff + 1;
          }
     }

     while (r != l) {
          i32 m = ((r - l) / 2) + l;
          i32 idx = m;
          idx = m + cache->start;
          cache_clamp_index(cache, &idx);
          if ( cache->buffer[idx].text == search ) {
               result = idx;
               break;
          }
          else if (search < cache->buffer[idx].text) {
               // Search on left
               r = m;
          }
          else {
               // Search on right
               l = m + 1;
          }
     }
#ifdef TOKENIZER_DEBUG
     i32 debug_result = __debug_cache_find_bin_search(cache, search);
     assert(result == debug_result);
     return result;
#endif
}



static inline void
cache_fill_token (Tokenizer_Cache *cache, Token *token)
{
     if (cache->at == cache->end) {
          cache_inc_index(cache, &cache->end);
          if (cache->end == cache->start ) {
               // End eats the start cuz newer token
               cache_inc_index(cache, &cache->start);
          }
     }
     cache->buffer[cache->at] = *token;
     cache_inc_index(cache, &cache->at);

     // TODO: Remove this testing piece of crap
     cache_find_bin_search(cache, token->text);
}

static inline void
cache_get_prev_token (Tokenizer_Cache *cache,
                        __OUT__ i32* prev_parsed_index )
{
     *prev_parsed_index = -1;
     // at       points to the place where the next token will be
     // at - 1   points to the place where the last parsed token is
     // at - 2   points where the prev parsed token reside
     if (cache->at == cache->start) {
          // No prev token avail to pop
     }
     else {
          i32 temp = cache->at;
          cache_dec_index(cache, &temp);
          if (temp != cache->start ) {
               i32 temp2 = temp;
               cache_dec_index(cache, &temp2);
               *prev_parsed_index = temp2;
          }
     }
}


static inline void
cache_get_next_token (Tokenizer_Cache *cache,
                      __OUT__ i32* next_parsed_index)
{

     *next_parsed_index = -1;
     // at       points to the place where the next token will be
     // at - 1   points to the place where the last parsed token is
     // at - 2   points where the prev parsed token reside

     if ( cache->at != cache->end ) {
          *next_parsed_index = cache->at;
     }
}


void
tokenizer_push_state ( Tokenizer *tknzr )
{
#if TOKENIZER_STATE_LIST_ENABLED
     Tokenizer_State_List *it = tknzr->list;
     assert (it);
     while ( it->used == TOKENIZER_MAX_NUM_STATES ) {
          if ( it->next ) {
               it = it->next;
          }
          else {
               // allocate new page
               it->next = alloc_memmapped_pages(0x0, sizeof(*(tknzr->list)), PAGE_PROT_WRITE | PAGE_PROT_READ,
                                                PAGE_PRIVATE | PAGE_ANONYMOUS);
               assert(it->next);
               it = it->next;
          }
     }
     it->states[it->used].at = tknzr->at;
     it->states[it->used].line_num = tknzr->line_num;
     it->states[it->used].column = tknzr->column;
     ++(it->used);
#endif
}



static void
cache_clear_invalidate(Tokenizer_Cache *cache )
{
     cache->at = 0;
     cache->start = 0;
     cache->end = 0;
}




bool
tokenizer_pop_state ( Tokenizer *tknzr )
{
     bool result = false;
#if TOKENIZER_STATE_LIST_ENABLED
     Tokenizer_State_List *it = tknzr->list;
     assert( it->used > 0 );
     assert (it);

     while ( true ) {
          if ( it->used != TOKENIZER_MAX_NUM_STATES
               || ((it->used == TOKENIZER_MAX_NUM_STATES) && (it->next == NULL))) {
               break;
          }
          else if ( it->next ) {
               if ( it->next->used == 0 ) {
                    break;
               }
               else {
                    it = it->next;
               }
          }
          else {
               assert_msg(0, "Invalid case should never happen");
          }
     }
     assert ( it->used != 0 );

     Tokenizer_State *state = &(it->states[it->used - 1]);

#if TOKENIZER_CACHING_ENABLED
     { // Cache recovery
         Tokenizer_Cache *cache = &tknzr->cache;
          int find_idx = cache_find_bin_search(cache, state->at);
          if ( find_idx == -1 ) {
               cache_clear_invalidate(cache);
          }
          else {
               cache->at = find_idx;
          }

     }
#endif

     // Restore the tokenizer to the required state
     {
          tknzr->at = state->at;
          tknzr->line_num = state->line_num;
          tknzr->column = state->column;
     }

     --(it->used);
     if ( it->used == 0 ) {
          if ( it->next) {
               int result = dealloc_memmapped(it->next, sizeof(*it));
               assert(result == 0);
               it->next = 0x0000;
          }
     }
#endif     
     return result;
}


bool
get_next_token ( Tokenizer *tknzr,
                 Token *token )
{

     Token local_token_for_null;

     token = token == NULL ? &local_token_for_null : token;

     i32 next_token_idx = -1;
# if TOKENIZER_CACHING_ENABLED
     Tokenizer_Cache *cache = &tknzr->cache;
     cache_get_next_token(cache, &next_token_idx);

     if ( next_token_idx != -1) {
          *token = cache->buffer[next_token_idx];
          cache_inc_index(cache, &cache->at);

          if (cache->at != cache->end ) {
               // Little optimization if we have even the token next to the next
               // debug_break();
               tknzr->line_num = cache->buffer[cache->at].line_num;
               tknzr->column = cache->buffer[cache->at].column;
               tknzr->at = cache->buffer[cache->at].text;
          }
          else {
               tknzr->line_num = cache->buffer[next_token_idx].line_num;
               tknzr->column = cache->buffer[next_token_idx].column + (cache->buffer[next_token_idx].text_len);
               tknzr->at = (cache->buffer[next_token_idx].text +
                          cache->buffer[next_token_idx].text_len);
               eat_whitespaces(tknzr);
          }
          // NOTE: No need to check to increment the cache->end and the cache->start
          //       by definition if we can get the token out that means that the end
          //       is offset by +1 and there's enough room for incrementing the at
     }
     else
#endif
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
          } else if ( is_punctuator (*tknzr->at )) {
               parse_punctuator(tknzr, token);
          } else if ( is_alpha (*tknzr->at) || *tknzr->at == '_' ) {
               parse_identifier_or_keyword ( tknzr, token );
          }
          eat_whitespaces(tknzr);
#if TOKENIZER_CACHING_ENABLED
          cache_fill_token(cache, token);
#endif
     }
     return true;
}



static bool
get_prev_token_from_cache_if_avail(Tokenizer *tknzr, Token *token )
{
#if TOKENIZER_CACHING_ENABLED
     bool result = false;
     Tokenizer_Cache *cache = &tknzr->cache;
     i32 prev_parsed_idx;
     cache_get_prev_token(cache, &prev_parsed_idx );
     if ( prev_parsed_idx != -1 ) {
          *token = cache->buffer[prev_parsed_idx];
          cache_dec_index(cache, &cache->at);
          if ( cache->at != cache->end ) {
               tknzr->line_num = cache->buffer[cache->at].line_num;
               tknzr->column = cache->buffer[cache->at].column;
               tknzr->at = cache->buffer[cache->at].text;
          }
          else {
               invalid_code_path();
               tknzr->line_num = cache->buffer[prev_parsed_idx].line_num;
               tknzr->column = (cache->buffer[prev_parsed_idx].column
                              + cache->buffer[prev_parsed_idx].text_len);
               tknzr->at = (cache->buffer[prev_parsed_idx].text +
                          cache->buffer[prev_parsed_idx].text_len);
               eat_whitespaces(tknzr);
          }
          result = true;
     }
     return result;
#else
     return false;
#endif
}

bool
get_prev_token ( Tokenizer *tknzr,
                 Token *token )
{
     Token local_token_for_null;
     token = token == NULL ? &local_token_for_null : token;
     bool result = false;

     if( tknzr->at == tknzr->base ) {
          debug_break();
          return false;
     }

     bool cached = get_prev_token_from_cache_if_avail(tknzr, token);
     if ( cached ) {
          result = true;
     }

     else {
          char *prev_tknzr_at = tknzr->at;
          assert_msg ( 0, "Test me" );
#if TOKENIZER_CACHING_ENABLED
          Tokenizer_Cache *cache = &tknzr->cache;
          cache_clear_invalidate(cache);
#endif

          // traverse back until newline not preceded by \ backquote
          i32 line_counter_bwd = 0;
          while (tknzr->at != tknzr->base) {
               if (is_newline_bwd(tknzr) && *(tknzr->at-1) != '\\') {
                    line_counter_bwd++;
                    if ( line_counter_bwd >= 4 ) {
                         tknzr->column = 0;
                         break;
                    }
               }
               --(tknzr->at);
          }

          // restore line counting and other stuff like that
          //     { should be already done in the upper loop }



          // restart normal parsing forward until we hit the previous token
          Token temp_token;
          while(get_next_token( tknzr, &temp_token )) {
               assert(!tknzr->err);
               if ( temp_token.text == prev_tknzr_at ) {
                    break;
               }
          }
          bool cached_ = get_prev_token_from_cache_if_avail(tknzr, token);
          // if can't find it inside the cache probably we are at the start
          // there's actually no prev token
          if ( cached_ ) {
               result = true;
          }
          else {
               result = false;
          }


#         if 0
          {
               // this implementatiion starts over from the start
               // Really expensive operation when we not hit the cache,
               // for now the simplest thing to do is to restart from
               // the start and go up refilling the cache
               // The cache does not contain the requisted token
               // Needs to invalidate the cache
               Tokenizer_State initial_state; (void) initial_state; {
                    initial_state.at = tknzr->at;
                    initial_state.line_num = tknzr->line_num;
                    initial_state.column = tknzr->column;
               };
               assert_msg ( 0, "Test me" );
               {
                    char *prev_tknzr_at = tknzr->at;
                    tknzr->at = tknzr->base;
                    tknzr->line_num = 0;
                    tknzr->column = 0;
                    cache_clear_invalidate(cache);

                    Token temp_token;
                    while(get_next_token( tknzr, &temp_token )) {
                         assert(!tknzr->err);
                         if ( temp_token.text == prev_tknzr_at ) {
                              break;
                         }
                    }
                    bool cached_ = get_prev_token_from_cache_if_avail(tknzr, token);
                    // if can't find it inside the cache probably we are at the start
                    // there's actually no prev token
                    if ( cached_ ) {
                         result = true;
                    }
                    else {
                         result = false;
                    }
               }
          }
#         endif
     }

     return result;
}


bool
discard_next_token ( Tokenizer* tok )
{
     return get_next_token ( tok, NULL );
}



bool
peek_next_token ( Tokenizer *tknzr,
                 Token *token )
{
     bool result = false;
     result = get_next_token(tknzr, token);
     bool temp = get_prev_token(tknzr, NULL);
     assert(temp);
     return result;
}


bool
peek_prev_token ( Tokenizer *tknzr,
                 Token *token )
{
     bool result = false;
     result = get_prev_token(tknzr, token);
     bool temp = get_next_token(tknzr, NULL);
     assert(temp);
     return result;
}


static inline void
cache_init (Tokenizer_Cache *cache)
{
#if TOKENIZER_CACHING_ENABLED
     cache->buffer_len = 512;
     size_t buffer_size = cache->buffer_len * sizeof(cache->buffer[0]);
     cache->buffer = alloc_memmapped_pages( 0x0, buffer_size,
                                            PAGE_PROT_READ | PAGE_PROT_WRITE,
                                            PAGE_PRIVATE | PAGE_ANONYMOUS );
     assert ( cache->buffer );

     cache_clear_invalidate(cache);
#endif
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

    // For the state stack allocate 2 linked mmap pages
#if TOKENIZER_STATE_LIST_ENABLED
    {
        tknzr->list = alloc_memmapped_pages( 0x0, sizeof(*(tknzr->list)), PAGE_PROT_READ | PAGE_PROT_WRITE,
                                             PAGE_PRIVATE | PAGE_ANONYMOUS);
        tknzr->list->next = 0x0000;
        assert ( tknzr->list );
    }
#endif
    
#if TOKENIZER_CACHING_ENABLED
    cache_init (&tknzr->cache);
#endif

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




void log_token (Token *token)
{
     printf("%.*s\tline_number=%d\tcolumn=%d\n", token->text_len, token->text, token->line_num, token->column);
}



bool
token_is_operator(Token *t)
{
    bool result = true;
    if ( t->type > TT_PUNCT_ENUM_OPERATORS_START_MARKER &&
         t->type < TT_PUNCT_ENUM_OPERATORS_END_MARKER ) {
        result = true;
    } else {
        result = false;
    }
    return result;
}


#endif /* PARSER_C_IMPL */
