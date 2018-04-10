#ifndef LANGUAGE_C_INCLUDE
#define LANGUAGE_C_INCLUDE


//#######################################################
#endif /* LANGUAGE_C_INCLUDE */
#if !defined LANGUAGE_C_IMPLEMENTED && defined LANGUAGE_C_IMPL
#define LANGUAGE_C_IMPLEMENTED
//#######################################################

enum operator_associativity {
    NON_ASSOCIATIVE_OP,
    RIGHT_ASSOCIATIVE_OP,
    LEFT_ASSOCIATIVE_OP,
};

enum operator_prefixing {
    PREFIX_OP,
    INFIX_OP,
    POSTFIX_OP,
};

static const struct operator_infos {
    int precedence;
    uint numofoperands;
    enum operator_associativity associativity;
    enum operator_prefixing prefixing;
} OPS[] = {
    [TT_PUNCT_SEMICOLON]  = { 16, 1, LEFT_ASSOCIATIVE_OP, POSTFIX_OP },
    [TT_PUNCT_COMMA]      = { 15, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },

    // C++ precedence: Place the ternary operator to the same precedence of equal
    //                 to avoid parsing failures.
    [TT_PUNCT_COLON]         = { 14, 3, RIGHT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_QUESTION_MARK] = { 14, 1, RIGHT_ASSOCIATIVE_OP, POSTFIX_OP },
        
    [TT_PUNCT_EQUAL]         = { 14, 2, RIGHT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_LOGICAL_OR]    = { 12, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_LOGICAL_AND]   = { 11, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_BITWISE_OR]    = { 10, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_BITWISE_XOR]   = { 9, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_BITWISE_AND]   = { 8, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },

    [TT_PUNCT_EQUAL_EQUAL]   = { 7, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_NOT_EQUAL ]    = { 7, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },

    // Place to insert bigger, bigger or equal, less, less or equal
    /* {   } */
    [TT_PUNCT_GREATER]          = {6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_LESS]             = {6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_GREATER_OR_EQUAL] = {6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_LESS_OR_EQUAL]    = {6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },


    [TT_PUNCT_META_DEREF]    = { 2, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },
    [TT_PUNCT_LOGICAL_NOT]   = { 2, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },
    [TT_PUNCT_BITWISE_NOT]   = { 2, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },

    [TT_PUNCT_BOTHDIR_ARROW] = { 1, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    // C-SPEC defines the arrow to be left associative. In propositional
    // calculus we use implication `->` right associative
    [TT_PUNCT_ARROW]         = { 1, 2, RIGHT_ASSOCIATIVE_OP, INFIX_OP },


    [TT_PUNCT_META_FNCALL]     = { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_META_INDEX]      = { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_META_COMPOUND]   = { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },

    
    [TT_PUNCT_OPEN_BRACE]      = { 0, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },
    [TT_PUNCT_OPEN_BRACKET]    = { 0, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },
    
    // Not valid set of operator types.
    [0 ... TT_PUNCT_ENUM_OPERATORS_START_MARKER] = { -1, 0, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_ENUM_MARKER_NOT_IMPLEMENTED_OPERATORS ... TT_PUNCT_ENUM_LAST_VALUE ]  = { -1, 0, LEFT_ASSOCIATIVE_OP, INFIX_OP },
};

static inline int
operator_precedence(Token *t)
{
    assert(token_is_operator(t));
    return OPS[t->type].precedence;
}

static inline bool
op_is_left_associative(Token *t)
{
    assert(token_is_operator(t));
    return (OPS[t->type].associativity == LEFT_ASSOCIATIVE_OP);
}

static inline bool
is_prefix_operator(Token *t)
{
    assert(token_is_operator(t));
    return (OPS[t->type].prefixing == PREFIX_OP);
}

static inline bool
is_postfix_operator(Token *t)
{
    assert(token_is_operator(t));
    return (OPS[t->type].prefixing == POSTFIX_OP);
}

static inline bool
is_infix_operator(Token *t)
{
    assert(token_is_operator(t));
    return (OPS[t->type].prefixing == INFIX_OP);
}


static inline bool
op_greater_precedence(Token *sample,
                      Token *tested )
{
    int p1 = operator_precedence(sample);
    int p2 = operator_precedence(tested);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 < p2;
}

static inline bool
op_eq_precedence(Token *sample,
                 Token *tested )
{
    int p1 = operator_precedence(sample);
    int p2 = operator_precedence(tested);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 == p2;
}


static inline uint
operator_numofoperands(Token *t)
{
    assert(token_is_operator(t));
    return OPS[t->type].numofoperands;
    return 0;
}








#endif /* LANGUAGE_C_IMPL */
