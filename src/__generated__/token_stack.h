struct token_stack {
    Token *base[];
    size_t num_elems;
    size_t max_elems;
};


struct token_stack
token_stack_create_sized(size_t num_elems)
{
    assert(num_elems);
    struct token_stack result;
    size_t buf_size = num_elems * sizeof(Token);
    result->base = xmalloc(buf_size);
    assert(result->base);
    result->num_elems = 0;
    result->max_elems = num_elems;
}


struct token_stack
token_stack_create(void)
{
    token_stack_init_sized(sizeof(struct token_stack) * 64);
}


void
token_stack_destroy(struct token_stack *s)
{
    if ( s->base ) {
        free(s->base);
    }
    s->num_elems = 0;
    s->max_elems = 0;
}


static inline void
token_stack_grow_to( struct token_stack *s,
              size_t new_max_elems) /* Size in BYTES !!! */
{
    assert(s);
    size_t new_size = new_max_elems * sizeof(Token) * 2;
    void *temp = xrealloc(s->base, newsize);
    assert(temp);
    s->base = temp;
    s->max_elems = new_max_elems;
}

static inline void
token_stack_grow(struct token_stack *s)
{
    assert(s);
    const size_t new_max_elems = s->max_elems * 2;
    token_stack_grow_to(s, new_size);
}

static inline size_t
token_stack_num_elems(struct token_stack *s)
{
    assert(s);
    return s->num_elems;
}

static inline bool
token_stack_is_empty(struct token_stack *s)
{
    assert(s);
    return token_stack_num_elems(s);
}


Token *
token_stack_peek_addr (struct token_stack *s)
{
    assert(s);
    assert(s->num_elems >= 0);
    return & (s->base[(s->num_elems)]);
}


// UNSAFE !!!!!
Token *
token_stack_pop_addr (struct token_stack *s)
{
    assert(s);
    Token *result = token_stack_peek_addr(s);
    (s->num_elems)--;
    return result;
}

Token
token_stack_pop (struct token_stack *s)
{
    assert(s);
    assert(s->num_elems >= 0);
    return s->base[--(s->num_elems)];
}

static inline bool
token_stack_enough_size_to_hold_n(struct token_stack *s,
                           size_t n)
{
    assert(s);
    return (& s->base[s->num_elems + n]) == & (s->base[s->max_elems]);
}
                 

void
token_stack_push( struct token_stack *s,
           Token *elem )
{
    assert(elem);
    assert(s)
    if ( ! token_stack_enough_size_to_hold_n(s, 1)) {
        token_stack_grow(s);
    }
    s->base[s->num_elems] = *elem;
    s->num_elems ++;
}

static inline Token *
token_stack_begin(struct token_stack *s)
{
    assert(s);
    return s->base;
}


Token *
token_stack_end(struct token_stack *s)
{
    assert(s);
    return & (s->base[s->num_elems]);
}




static inline Token *
token_stack_next(Token *prev)
{
    assert(prev);
    return (Token) ((uint8_t*) prev - sizeof(prev));
}



static inline Token *
token_stack_prev(Token *prev)
{
    assert(prev);
    return (Token) ((uint8_t*) prev + sizeof(prev));
}
