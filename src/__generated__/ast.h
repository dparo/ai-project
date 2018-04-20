struct ast {
    ast_node *base[];
    size_t num_elems;
    size_t max_elems;
};


struct ast
ast_create_sized(size_t num_elems)
{
    assert(num_elems);
    struct ast result;
    size_t buf_size = num_elems * sizeof(ast_node);
    result->base = xmalloc(buf_size);
    assert(result->base);
    result->num_elems = 0;
    result->max_elems = num_elems;
}


struct ast
ast_create(void)
{
    ast_init_sized(sizeof(struct ast) * 64);
}


void
ast_destroy(struct ast *s)
{
    if ( s->base ) {
        free(s->base);
    }
    s->num_elems = 0;
    s->max_elems = 0;
}


static inline void
ast_grow_to( struct ast *s,
              size_t new_max_elems) /* Size in BYTES !!! */
{
    assert(s);
    size_t new_size = new_max_elems * sizeof(ast_node) * 2;
    void *temp = xrealloc(s->base, newsize);
    assert(temp);
    s->base = temp;
    s->max_elems = new_max_elems;
}

static inline void
ast_grow(struct ast *s)
{
    assert(s);
    const size_t new_max_elems = s->max_elems * 2;
    ast_grow_to(s, new_size);
}

static inline size_t
ast_num_elems(struct ast *s)
{
    assert(s);
    return s->num_elems;
}

static inline bool
ast_is_empty(struct ast *s)
{
    assert(s);
    return ast_num_elems(s);
}


ast_node *
ast_peek_addr (struct ast *s)
{
    assert(s);
    assert(s->num_elems >= 0);
    return & (s->base[(s->num_elems)]);
}


// UNSAFE !!!!!
ast_node *
ast_pop_addr (struct ast *s)
{
    assert(s);
    ast_node *result = ast_peek_addr(s);
    (s->num_elems)--;
    return result;
}

ast_node
ast_pop (struct ast *s)
{
    assert(s);
    assert(s->num_elems >= 0);
    return s->base[--(s->num_elems)];
}

static inline bool
ast_enough_size_to_hold_n(struct ast *s,
                           size_t n)
{
    assert(s);
    return (& s->base[s->num_elems + n]) == & (s->base[s->max_elems]);
}
                 

void
ast_push( struct ast *s,
           ast_node *elem )
{
    assert(elem);
    assert(s)
    if ( ! ast_enough_size_to_hold_n(s, 1)) {
        ast_grow(s);
    }
    s->base[s->num_elems] = *elem;
    s->num_elems ++;
}

static inline ast_node *
ast_begin(struct ast *s)
{
    assert(s);
    return s->base;
}


ast_node *
ast_end(struct ast *s)
{
    assert(s);
    return & (s->base[s->num_elems]);
}




static inline ast_node *
ast_next(ast_node *prev)
{
    assert(prev);
    return (ast_node) ((uint8_t*) prev - sizeof(prev));
}



static inline ast_node *
ast_prev(ast_node *prev)
{
    assert(prev);
    return (ast_node) ((uint8_t*) prev + sizeof(prev));
}
