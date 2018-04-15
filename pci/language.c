#ifndef LANGUAGE_C_INCLUDE
#define LANGUAGE_C_INCLUDE


//#######################################################
#endif /* LANGUAGE_C_INCLUDE */
#if !defined LANGUAGE_C_IMPLEMENTED && defined LANGUAGE_C_IMPL
#define LANGUAGE_C_IMPLEMENTED
//#######################################################

enum operator {
    OPERATOR_NONE = 0,
    OPERATOR_NEGATE,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_XOR,
    OPERATOR_IMPLY,
    OPERATOR_DOUBLE_IMPLY,
 
    OPERATOR_EQUAL,
    OPERATOR_NOT_EQUAL,
    OPERATOR_GREATER,
    OPERATOR_GREATER_EQUAL,
    OPERATOR_LESS,
    OPERATOR_LESS_EQUAL,

    OPERATOR_ASSIGN,
    OPERATOR_CONCAT_AND_ASSIGN,
    OPERATOR_CONCAT,

    OPERATOR_DEREF,
    OPERATOR_FNCALL,
    OPERATOR_INDEX,
    OPERATOR_COMPOUND,

    OPERATOR_ENUMERATE,
    OPERATOR_EXIST,
    OPERATOR_IN,
    
    OPERATOR_TERNARY,
};

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
    [OPERATOR_NONE] =              { INT_MIN, 0, 0, POSTFIX_OP},
    [OPERATOR_NEGATE] =            { 2, 1, LEFT_ASSOCIATIVE_OP, PREFIX_OP },
    [OPERATOR_AND] =               { 11, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_OR] =                { 12, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_XOR] =               { 9, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_IMPLY] =             { 1, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_DOUBLE_IMPLY] =      { 1, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_EQUAL] =             { 7, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_NOT_EQUAL] =         { 7, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_GREATER] =           { 6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_GREATER_EQUAL] =     { 6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_LESS] =              { 6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_LESS_EQUAL] =        { 6, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_ASSIGN] =            { 14, 2, RIGHT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_CONCAT_AND_ASSIGN] = { 14, 2, RIGHT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_CONCAT] =            { 2, 2,  LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_DEREF] =             { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_FNCALL] =            { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_INDEX] =             { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_COMPOUND] =          { 0, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },
    [OPERATOR_ENUMERATE] =         { 14, 2, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
    [OPERATOR_EXIST] =             { 14, 2, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
    [OPERATOR_IN] =                { 13, 1, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
    [OPERATOR_TERNARY] =           { 14, 3, RIGHT_ASSOCIATIVE_OP, POSTFIX_OP },

#if 0
    [TT_PUNCT_SEMICOLON]  = { 16, 1, LEFT_ASSOCIATIVE_OP, POSTFIX_OP },
    [TT_PUNCT_COMMA]      = { 15, 2, LEFT_ASSOCIATIVE_OP, INFIX_OP },

    // C++ precedence: Place the ternary operator to the same precedence of equal
    //                 to avoid parsing failures.
    [TT_PUNCT_COLON]         = { 14, 3, RIGHT_ASSOCIATIVE_OP, INFIX_OP },
    [TT_PUNCT_QUESTION_MARK] = { 14, 1, RIGHT_ASSOCIATIVE_OP, POSTFIX_OP },
    [TT_PUNCT_POUND]         = { 14, 1, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
    [TT_PUNCT_DOLLAR_SIGN]   = { 14, 1, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
    [TT_PUNCT_AT_SIGN]       = { 13, 1, RIGHT_ASSOCIATIVE_OP, PREFIX_OP },
        
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
#endif
};


enum ast_node_type {
    AST_NODE_TYPE_NONE,
    AST_NODE_TYPE_OPERATOR,
    AST_NODE_TYPE_IDENTIFIER,
    AST_NODE_TYPE_KEYWORD,
    AST_NODE_TYPE_CONSTANT,
    AST_NODE_TYPE_DELIMITER,
};


enum delimiter {
    // It's not a delimiter
    DELIMITER_NONE,
    
    PREFIX_DELIMITER_PAREN = 1,
    PREFIX_DELIMITER_BRACKET,
    PREFIX_DELIMITER_BRACE,
    PREFIX_DELIMITER_QUESTION_MARK,

    INFIX_DELIMITER_COMMA = 1 << 8,

    POSTFIX_DELIMITER_PAREN = 1 << 16,
    POSTFIX_DELIMITER_BRACKET,
    POSTFIX_DELIMITER_BRACE,
    POSTFIX_DELIMITER_COLON,
    POSTFIX_DELIMITER_SEMICOLON,
};



struct ast_node {
    char *text;
    i32 text_len;
    int num_operands;
    enum ast_node_type type;
    enum operator op;
    enum delimiter del;
};




bool
ast_node_is_valid(struct ast_node *node)
{
    assert(node);
    return node->type != AST_NODE_TYPE_NONE;
}


bool
is_prefix_delimiter(struct ast_node *node)
{
    assert(node);
    assert(ast_node_is_valid(node));
    if ( (node->type == AST_NODE_TYPE_DELIMITER || node->type == AST_NODE_TYPE_OPERATOR)
         && (node->del != DELIMITER_NONE)) {
        enum delimiter d = node->del;
        int mask = 0;
        mask |= 1 << 8;
        mask |= 1 << 16;
        if ( (d & mask) == 0 ) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool
is_infix_delimiter(struct ast_node *node)
{
    assert(node);
    assert(ast_node_is_valid(node));
    if ( (node->type == AST_NODE_TYPE_DELIMITER || node->type == AST_NODE_TYPE_OPERATOR)
         && (node->del != DELIMITER_NONE)) {
        enum delimiter d = node->del;
        int mask = 0;
        mask |= 1 << 8;
        
        if ( (d & mask) != 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}



bool
is_postfix_delimiter(struct ast_node *node)
{
    assert(node);
    assert(ast_node_is_valid(node));
    if ( (node->type == AST_NODE_TYPE_DELIMITER || node->type == AST_NODE_TYPE_OPERATOR)
         && (node->del != DELIMITER_NONE)) {
        enum delimiter d = node->del;
        int mask = 0;
        mask |= 1 << 16;
        
        if ( (d & mask) != 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


static inline bool
is_delimiter(struct ast_node *node)
{
    bool result = false;
    result |= is_prefix_delimiter(node);
    result |= is_infix_delimiter(node);
    result |= is_postfix_delimiter(node);
    return result;
}


void
ast_node_invalidate(struct ast_node *node)
{
    assert(node);
    node->type = AST_NODE_TYPE_NONE;
}

void
ast_node_print( FILE *f, struct ast_node *node )
{
    assert(node);
    assert(ast_node_is_valid(node));
    
    FILE *stream = f ? f : stdout;
    char *text = node->text;
    int text_len = node->text_len;
   
    if ( node->op == OPERATOR_FNCALL ) {
        text = "`fncall`";
        text_len = sizeof("`fncall`") - 1;
    } else if ( node->op == OPERATOR_INDEX )  {
        text = "`index`";
        text_len = sizeof("`index`") - 1;
    } else if ( node->op == OPERATOR_COMPOUND ) {
        text = "`compound`";
        text_len = sizeof("`compound`") - 1;
    } else if ( node->op == OPERATOR_DEREF ) {
        text = "`deref`";
        text_len = sizeof("`deref`") - 1;
    } else if ( node->type == AST_NODE_TYPE_DELIMITER ) {
        if (node->del == DELIMITER_NONE) {
            text = "`__none__`";
            text_len = sizeof("`__none__`") - 1;
        } else if (node->del == PREFIX_DELIMITER_PAREN) {
            text = "`(`";
            text_len = sizeof("`(`") - 1;
        } else if (node->del == PREFIX_DELIMITER_BRACKET) {
            text = "`[`";
            text_len = sizeof("`[`") - 1;
        }else if (node->del == PREFIX_DELIMITER_BRACE) {
            text = "`{`";
            text_len = sizeof("`{`") - 1;

        } else if (node->del == INFIX_DELIMITER_COMMA ) {
            text = "`,`";
            text_len = sizeof("`,`") - 1;

        } else if (node->del == POSTFIX_DELIMITER_PAREN) {
            text = "`)`";
            text_len = sizeof("`)`") - 1;

        } else if (node->del == POSTFIX_DELIMITER_BRACKET) {
            text = "`]`";
            text_len = sizeof("]`") - 1;

        } else if (node->del == POSTFIX_DELIMITER_BRACE) {
            text = "`}`";
            text_len = sizeof("`}`") - 1;

        } else if (node->del == POSTFIX_DELIMITER_COLON) {
            text = "`:`";
            text_len = sizeof("`:`") - 1;

        } else if (node->del == POSTFIX_DELIMITER_SEMICOLON) {
            text = "`;`";
            text_len = sizeof("`;`") - 1;

        }
    }
                        
    fprintf(stream, "%.*s", text_len, text);
}


static inline bool
ast_node_is_operator (struct ast_node *node)
{
    assert(node);
    return node->type == AST_NODE_TYPE_OPERATOR;
}



static inline int
operator_precedence(struct ast_node *node)
{
    assert(ast_node_is_operator(node));
    return OPS[node->op].precedence;
}

static inline bool
op_is_left_associative(struct ast_node *node)
{
    assert(ast_node_is_operator(node));
    return (OPS[node->op].associativity == LEFT_ASSOCIATIVE_OP);
}

static inline bool
is_prefix_operator(struct ast_node *node)
{
    assert(ast_node_is_operator(node));
    return (OPS[node->op].prefixing == PREFIX_OP);
}

static inline bool
is_postfix_operator(struct ast_node *node)
{
    assert(ast_node_is_operator(node));
    return (OPS[node->op].prefixing == POSTFIX_OP);
}

static inline bool
is_infix_operator(struct ast_node *node)
{
    assert(ast_node_is_operator(node));
    return (OPS[node->op].prefixing == INFIX_OP);
}


static inline bool
op_greater_precedence(struct ast_node *n1,
                      struct ast_node *n2 )
{
    int p1 = operator_precedence(n1);
    int p2 = operator_precedence(n2);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 < p2;
}

static inline bool
op_eq_precedence(struct ast_node *n1,
                 struct ast_node *n2 )
{
    int p1 = operator_precedence(n1);
    int p2 = operator_precedence(n2);
    assert(p1 >= 0);
    assert(p2 >= 0);
    return p1 == p2;
}



static inline uint
operator_num_operands(struct ast_node *node)
{
    if ( ast_node_is_operator(node)) {
        return OPS[node->op].numofoperands;
    } else if (is_delimiter(node)) {
        return node->num_operands;
    } else {
        return 0;
    }
}


bool
ast_node_from_token( struct ast_node *node,
                     Token *curr_t,
                     Token *prev_t )
{
    assert(curr_t);
    
    bool result = true;

    node->text         = curr_t->text;
    node->text_len     = curr_t->text_len;
    node->op           = OPERATOR_NONE;
    node->del          = DELIMITER_NONE;

    if ( curr_t->type == TT_IDENTIFIER ) {
        node->type = AST_NODE_TYPE_IDENTIFIER;
    } else if ( curr_t->type == TT_KEYWORD ) {
        if ( strncmp ("in", curr_t->text, curr_t->text_len) == 0) {
            node->type = AST_NODE_TYPE_OPERATOR;
            node->op = OPERATOR_IN;
        } else {
            node->type = AST_NODE_TYPE_KEYWORD;
        }
    } else if (curr_t->type == TT_CONSTANT ) {
        node->type = AST_NODE_TYPE_CONSTANT;
    }

    else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        if (prev_t && prev_t->type == TT_IDENTIFIER) {
            node->type = AST_NODE_TYPE_OPERATOR;
            node->op = OPERATOR_FNCALL;
        } else {
            node->type = AST_NODE_TYPE_DELIMITER;
        }
        node->del = PREFIX_DELIMITER_PAREN;
    } else if (curr_t->type == TT_PUNCT_OPEN_BRACKET) {
        if (prev_t && prev_t->type == TT_IDENTIFIER) {
            node->type = AST_NODE_TYPE_OPERATOR;
            node->op = OPERATOR_INDEX;
        } else {
            node->type = AST_NODE_TYPE_DELIMITER;
        }
        node->del = PREFIX_DELIMITER_BRACKET;
    } else if (curr_t->type == TT_PUNCT_OPEN_BRACE) {
        if (prev_t && prev_t->type == TT_IDENTIFIER) {
            node->type = AST_NODE_TYPE_OPERATOR;
            node->op = OPERATOR_COMPOUND;
        } else {
            node->type = AST_NODE_TYPE_DELIMITER;
        }
        node->del = PREFIX_DELIMITER_BRACE;
    }

    else if (curr_t->type == TT_PUNCT_CLOSE_PAREN) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = POSTFIX_DELIMITER_PAREN;
    } else if (curr_t->type == TT_PUNCT_CLOSE_BRACKET) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = POSTFIX_DELIMITER_BRACKET;
    } else if (curr_t->type == TT_PUNCT_CLOSE_BRACE) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = POSTFIX_DELIMITER_BRACE;
    }

    
    else if (curr_t->type == TT_PUNCT_COMMA) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = INFIX_DELIMITER_COMMA;
    } else if (curr_t->type == TT_PUNCT_SEMICOLON) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = POSTFIX_DELIMITER_SEMICOLON;
    } else if (curr_t->type == TT_PUNCT_QUESTION_MARK) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = PREFIX_DELIMITER_QUESTION_MARK;
    } else if (curr_t->type == TT_PUNCT_COLON) {
        node->type = AST_NODE_TYPE_DELIMITER;
        node->del = POSTFIX_DELIMITER_COLON;
    }


    
    
    else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    } else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    } else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    } else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    }  else if (curr_t->type == TT_PUNCT_OPEN_PAREN) {
        
    } else {
        node->type = AST_NODE_TYPE_OPERATOR;
        switch( curr_t->type ) {
        case TT_PUNCT_LOGICAL_NOT: case TT_PUNCT_BITWISE_NOT: { node->op = OPERATOR_NEGATE; } break;
        case TT_PUNCT_LOGICAL_AND: case TT_PUNCT_BITWISE_AND: { node->op = OPERATOR_AND; } break;
        case TT_PUNCT_LOGICAL_OR: case TT_PUNCT_BITWISE_OR: { node->op = OPERATOR_OR; } break;
        case TT_PUNCT_BITWISE_XOR: { node->op = OPERATOR_XOR; } break;
        case TT_PUNCT_ARROW: { node->op = OPERATOR_IMPLY; } break;
        case TT_PUNCT_BOTHDIR_ARROW: { node->op = OPERATOR_DOUBLE_IMPLY; } break;
        case TT_PUNCT_EQUAL: { node->op = OPERATOR_ASSIGN; } break;
        case TT_PUNCT_PLUS: { node->op = OPERATOR_CONCAT; } break;
        case TT_PUNCT_ADDITION_EQUAL: { node->op = OPERATOR_CONCAT_AND_ASSIGN; } break;
        case TT_PUNCT_EQUAL_EQUAL: { node->op = OPERATOR_EQUAL; } break;
        case TT_PUNCT_NOT_EQUAL: { node->op = OPERATOR_NOT_EQUAL; } break;
        case TT_PUNCT_GREATER: { node->op = OPERATOR_GREATER; } break;
        case TT_PUNCT_GREATER_OR_EQUAL: { node->op = OPERATOR_GREATER_EQUAL; } break;
        case TT_PUNCT_LESS: { node->op = OPERATOR_LESS; } break;
        case TT_PUNCT_LESS_OR_EQUAL: { node->op = OPERATOR_LESS_EQUAL; } break;
        case TT_PUNCT_POUND: { node->op = OPERATOR_ENUMERATE; } break;
        case TT_PUNCT_DOLLAR_SIGN: { node->op = OPERATOR_EXIST; } break;
        case TT_PUNCT_AT_SIGN: { node->op = OPERATOR_IN; } break;


            // @ TODO: REVISIT TERNARY OPERATORS
        case TT_PUNCT_QUESTION_MARK: case TT_PUNCT_COLON: {
            assert_msg(0, "Ternary operator needs testing\n");
            node->op = OPERATOR_TERNARY;
        } break;            

        case TT_PUNCT_ASTERISK: {
            if ( !prev_t || (prev_t->type != TT_IDENTIFIER
                             && prev_t->type != TT_CONSTANT
                             && prev_t->type != TT_PUNCT_CLOSE_PAREN)) {
                node->op = OPERATOR_DEREF;
            } else {
                ast_node_invalidate(node);
                result = false;
            }
        } break;

        default: {
            // Not understood operator
            ast_node_invalidate(node);
            result = false;
        } break;
        }
    }

    if ( result == true ) {
        assert(node->type != AST_NODE_TYPE_NONE);
        bool check = false;
        if ( node->type == AST_NODE_TYPE_IDENTIFIER ) check |= true;
        if ( node->type == AST_NODE_TYPE_CONSTANT ) check |= true;
        if ( node->type == AST_NODE_TYPE_OPERATOR && node->op != OPERATOR_NONE) check |= true;
        if ( node->type == AST_NODE_TYPE_DELIMITER && node->del != DELIMITER_NONE) check |= true;
        assert( check );
    }
    node->num_operands = operator_num_operands(node);
    
    return result;
          
}








#endif /* LANGUAGE_C_IMPL */
