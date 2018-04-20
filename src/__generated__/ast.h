struct ast {
    struct ast_node *nodes;
    size_t num_nodes;
    size_t max_nodes;
};


struct ast
ast_create_sized(size_t num_nodes)
{
    assert(num_nodes);
    struct ast result;
    size_t buf_size = num_nodes * sizeof(struct ast_node);
    result.nodes = xmalloc(buf_size);
    assert(result.nodes);
    result.num_nodes = 0;
    result.max_nodes = num_nodes;
    return result;
}


struct ast
ast_create(void)
{
    return ast_create_sized(64);
}


void
ast_clear(struct ast *s)
{
    if ( s->nodes ) {
        free(s->nodes);
    }
    s->num_nodes = 0;
    s->max_nodes = 0;
}


static inline void
ast_grow_to( struct ast *s,
              size_t new_max_nodes) /* Size in BYTES !!! */
{
    assert(s);
    size_t new_size = new_max_nodes * sizeof(struct ast_node) * 2;
    void *temp = xrealloc(s->nodes, new_size);
    assert(temp);
    s->nodes = temp;
    s->max_nodes = new_max_nodes;
}

static inline void
ast_grow(struct ast *s)
{
    assert(s);
    const size_t new_max_nodes = s->max_nodes * 2;
    ast_grow_to(s, new_max_nodes);
}


inline size_t
ast_num_nodes(struct ast *s)
{
    assert(s);
    return s->num_nodes;
}

inline bool
ast_is_empty(struct ast *s)
{
    assert(s);
    return ast_num_nodes(s);
}


inline struct ast_node *
ast_peek_addr (struct ast *s)
{
    assert(s);
    return & (s->nodes[(s->num_nodes) - 1]);
}


// UNSAFE !!!!!
inline struct ast_node *
ast_pop_addr (struct ast *s)
{
    assert(s);
    struct ast_node *result = ast_peek_addr(s);
    (s->num_nodes)--;
    return result;
}

inline struct ast_node
ast_pop (struct ast *s)
{
    assert(s);
    return s->nodes[--(s->num_nodes)];
}

inline struct ast_node
ast_pop_discard (struct ast *s)
{
    assert(s);
    --(s->num_nodes);
}


static inline bool
ast_enough_size_to_hold_n(struct ast *s,
                           size_t n)
{
    assert(s);
    return (& s->nodes[s->num_nodes + n]) == & (s->nodes[s->max_nodes]);
}
                 

void
ast_push( struct ast *s,
           struct ast_node *elem )
{
    assert(elem);
    assert(s);
    if ( ! ast_enough_size_to_hold_n(s, 1)) {
        ast_grow(s);
    }
    s->nodes[s->num_nodes] = *elem;
    s->num_nodes ++;
}

inline struct ast_node *
ast_begin(struct ast *s)
{
    assert(s);
    return s->nodes;
}


inline struct ast_node *
ast_end(struct ast *s)
{
    assert(s);
    return & (s->nodes[s->num_nodes]);
}




inline struct ast_node *
ast_next(struct ast_node *prev)
{
    assert(prev);
    return (struct ast_node*) ((uint8_t*) prev - sizeof(prev));
}



inline struct ast_node *
ast_prev(struct ast_node *prev)
{
    assert(prev);
    return (struct ast_node*) ((uint8_t*) prev + sizeof(prev));
}
