struct ast_node_parent_child_pair_stack {
    struct ast_node_child_parent_pair *pairs;
    size_t num_pairs;
    size_t max_pairs;
};


struct ast_node_parent_child_pair_stack
ast_node_parent_child_pair_stack_create_sized(size_t num_pairs)
{
    assert(num_pairs);
    struct ast_node_parent_child_pair_stack result;
    size_t buf_size = num_pairs * sizeof(struct ast_node_child_parent_pair);
    result.pairs = xmalloc(buf_size);
    assert(result.pairs);
    result.num_pairs = 0;
    result.max_pairs = num_pairs;
    return result;
}


struct ast_node_parent_child_pair_stack
ast_node_parent_child_pair_stack_create(void)
{
    return ast_node_parent_child_pair_stack_create_sized(64);
}


struct ast_node_parent_child_pair_stack
ast_node_parent_child_pair_stack_dup(struct ast_node_parent_child_pair_stack *s)
{
    struct ast_node_parent_child_pair_stack result;
    const size_t size = s->max_pairs * sizeof(struct ast_node_child_parent_pair);
    result.pairs = xmalloc(size);
    assert(result.pairs);

    result.num_pairs = s->num_pairs;
    result.max_pairs = s->max_pairs;

    memcpy(result.pairs, s->pairs, s->num_pairs * sizeof(struct ast_node_child_parent_pair));
    return result;
}

void
ast_node_parent_child_pair_stack_free(struct ast_node_parent_child_pair_stack *s)
{
    if ( s->pairs ) {
        free(s->pairs);
    }
    s->pairs = 0;
    s->num_pairs = 0;
    s->max_pairs = 0;
}

void
ast_node_parent_child_pair_stack_clear(struct ast_node_parent_child_pair_stack *s)
{
    ast_node_parent_child_pair_stack_free(s);
}


void
ast_node_parent_child_pair_stack_reset(struct ast_node_parent_child_pair_stack *s)
{
    s->num_pairs = 0;
}



static inline void
ast_node_parent_child_pair_stack_grow_to( struct ast_node_parent_child_pair_stack *s,
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
ast_node_parent_child_pair_stack_grow(struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    const size_t new_max_pairs = s->max_pairs * 2;
    ast_node_parent_child_pair_stack_grow_to(s, new_max_pairs);
}


static inline size_t
ast_node_parent_child_pair_stack_num_pairs(struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    return s->num_pairs;
}

static inline bool
ast_node_parent_child_pair_stack_is_empty(struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    return ast_node_parent_child_pair_stack_num_pairs(s) == 0;
}


static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_peek_addr (struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    if (!(s->num_pairs)) {
        return (struct ast_node_child_parent_pair *) 0x00;
    }
    return & (s->pairs[(s->num_pairs) - 1]);
}


// UNSAFE !!!!!
static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_pop_addr (struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    struct ast_node_child_parent_pair *result = ast_node_parent_child_pair_stack_peek_addr(s);
    if ( result ) {
        (s->num_pairs)--;
    }
    return result;
}

static inline struct ast_node_child_parent_pair
ast_node_parent_child_pair_stack_pop (struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    assert((s->num_pairs));
    return s->pairs[--(s->num_pairs)];
}

static inline void
ast_node_parent_child_pair_stack_pop_discard (struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    assert(s->num_pairs);
    --(s->num_pairs);
}


static inline bool
ast_node_parent_child_pair_stack_enough_size_to_hold_n(struct ast_node_parent_child_pair_stack *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->num_pairs + n) <= (s->max_pairs));
}
                 

void
ast_node_parent_child_pair_stack_push( struct ast_node_parent_child_pair_stack *s,
           struct ast_node_child_parent_pair * elem )
{
    assert(elem);
    assert(s);
    if ( ! ast_node_parent_child_pair_stack_enough_size_to_hold_n(s, 1)) {
        ast_node_parent_child_pair_stack_grow(s);
    }
    s->pairs[s->num_pairs] = * elem;
    s->num_pairs ++;
}

static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_begin(struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    return s->pairs;
}


static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_end(struct ast_node_parent_child_pair_stack *s)
{
    assert(s);
    return & (s->pairs[s->num_pairs]);
}




static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_next(struct ast_node_child_parent_pair *prev)
{
    assert(prev);
    return (struct ast_node_child_parent_pair*) ((uint8_t*) prev - sizeof(prev));
}



static inline struct ast_node_child_parent_pair *
ast_node_parent_child_pair_stack_prev(struct ast_node_child_parent_pair *prev)
{
    assert(prev);
    return (struct ast_node_child_parent_pair*) ((uint8_t*) prev + sizeof(prev));
}
