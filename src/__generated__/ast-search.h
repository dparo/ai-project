struct ast_search {
    struct ast_node_child_parent_pair *pairs;
    size_t num_pairs;
    size_t max_pairs;
};


struct ast_search
ast_search_create_sized(size_t num_pairs)
{
    assert(num_pairs);
    struct ast_search result;
    size_t buf_size = num_pairs * sizeof(struct ast_node_child_parent_pair);
    result.pairs = xmalloc(buf_size);
    assert(result.pairs);
    result.num_pairs = 0;
    result.max_pairs = num_pairs;
    return result;
}


struct ast_search
ast_search_create(void)
{
    return ast_search_create_sized(64);
}


struct ast_search
ast_search_dup(struct ast_search *s)
{
    struct ast_search result;
    const size_t size = s->max_pairs * sizeof(struct ast_node_child_parent_pair);
    result.pairs = xmalloc(size);
    assert(result.pairs);

    result.num_pairs = s->num_pairs;
    result.max_pairs = s->max_pairs;

    memcpy(result.pairs, s->pairs, s->num_pairs * sizeof(struct ast_node_child_parent_pair));
    return result;
}

void
ast_search_free(struct ast_search *s)
{
    if ( s->pairs ) {
        free(s->pairs);
    }
    s->pairs = 0;
    s->num_pairs = 0;
    s->max_pairs = 0;
}

void
ast_search_clear(struct ast_search *s)
{
    ast_search_free(s);
}


void
ast_search_reset(struct ast_search *s)
{
    s->num_pairs = 0;
}



static inline void
ast_search_grow_to( struct ast_search *s,
              size_t new_max_pairs)
{
    assert(s);
    if ( new_max_pairs == 0 ) {
        new_max_pairs = 64;
    }
    size_t new_size = new_max_pairs * sizeof(struct ast_node_child_parent_pair) * 2;
    void *temp = xrealloc(s->pairs, new_size);
    assert(temp);
    s->pairs = temp;
    s->max_pairs = new_max_pairs;
}

static inline void
ast_search_grow(struct ast_search *s)
{
    assert(s);
    const size_t new_max_pairs = s->max_pairs * 2;
    ast_search_grow_to(s, new_max_pairs);
}


static inline size_t
ast_search_num_pairs(struct ast_search *s)
{
    assert(s);
    return s->num_pairs;
}

static inline bool
ast_search_is_empty(struct ast_search *s)
{
    assert(s);
    return ast_search_num_pairs(s) == 0;
}


static inline struct ast_node_child_parent_pair *
ast_search_peek_addr (struct ast_search *s)
{
    assert(s);
    if (!(s->num_pairs)) {
        return (struct ast_node_child_parent_pair *) 0x00;
    }
    return & (s->pairs[(s->num_pairs) - 1]);
}


// UNSAFE !!!!!
static inline struct ast_node_child_parent_pair *
ast_search_pop_addr (struct ast_search *s)
{
    assert(s);
    struct ast_node_child_parent_pair *result = ast_search_peek_addr(s);
    if ( result ) {
        (s->num_pairs)--;
    }
    return result;
}

static inline struct ast_node_child_parent_pair
ast_search_pop (struct ast_search *s)
{
    assert(s);
    assert((s->num_pairs));
    return s->pairs[--(s->num_pairs)];
}

static inline void
ast_search_pop_discard (struct ast_search *s)
{
    assert(s);
    assert(s->num_pairs);
    --(s->num_pairs);
}


static inline bool
ast_search_enough_size_to_hold_n(struct ast_search *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->num_pairs + n) <= (s->max_pairs));
}
                 

void
ast_search_push( struct ast_search *s,
           struct ast_node_child_parent_pair * elem )
{
    assert(elem);
    assert(s);
    if ( ! ast_search_enough_size_to_hold_n(s, 1)) {
        ast_search_grow(s);
    }
    s->pairs[s->num_pairs] = * elem;
    s->num_pairs ++;
}

static inline struct ast_node_child_parent_pair *
ast_search_begin(struct ast_search *s)
{
    assert(s);
    return s->pairs;
}


static inline struct ast_node_child_parent_pair *
ast_search_end(struct ast_search *s)
{
    assert(s);
    return & (s->pairs[s->num_pairs]);
}




static inline struct ast_node_child_parent_pair *
ast_search_next(struct ast_node_child_parent_pair *prev)
{
    assert(prev);
    return (struct ast_node_child_parent_pair*) ((uint8_t*) prev - sizeof(prev));
}



static inline struct ast_node_child_parent_pair *
ast_search_prev(struct ast_node_child_parent_pair *prev)
{
    assert(prev);
    return (struct ast_node_child_parent_pair*) ((uint8_t*) prev + sizeof(prev));
}
