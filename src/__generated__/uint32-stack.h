struct uint32_stack {
    uint32_t *data;
    size_t num_elems;
    size_t max_elems;
};


struct uint32_stack
uint32_stack_create_sized(size_t num_elems)
{
    assert(num_elems);
    struct uint32_stack result;
    size_t buf_size = num_elems * sizeof(uint32_t);
    result.data = xmalloc(buf_size);
    assert(result.data);
    result.num_elems = 0;
    result.max_elems = num_elems;
    return result;
}


struct uint32_stack
uint32_stack_create(void)
{
    return uint32_stack_create_sized(64);
}


struct uint32_stack
uint32_stack_dup(struct uint32_stack *s)
{
    struct uint32_stack result;
    const size_t size = s->max_elems * sizeof(uint32_t);
    result.data = xmalloc(size);
    assert(result.data);

    result.num_elems = s->num_elems;
    result.max_elems = s->max_elems;

    memcpy(result.data, s->data, s->num_elems * sizeof(uint32_t));
    return result;
}

void
uint32_stack_free(struct uint32_stack *s)
{
    if ( s->data ) {
        free(s->data);
    }
    s->data = 0;
    s->num_elems = 0;
    s->max_elems = 0;
}

void
uint32_stack_clear(struct uint32_stack *s)
{
    uint32_stack_free(s);
}


void
uint32_stack_reset(struct uint32_stack *s)
{
    s->num_elems = 0;
}



static inline void
uint32_stack_grow_to( struct uint32_stack *s,
              size_t new_max_elems)
{
    assert(s);
    if ( new_max_elems == 0 ) {
        new_max_elems = 64;
    }
    size_t new_size = new_max_elems * sizeof(uint32_t) * 2;
    void *temp = xrealloc(s->data, new_size);
    assert(temp);
    s->data = temp;
    s->max_elems = new_max_elems;
}

static inline void
uint32_stack_grow(struct uint32_stack *s)
{
    assert(s);
    const size_t new_max_elems = s->max_elems * 2;
    uint32_stack_grow_to(s, new_max_elems);
}


static inline size_t
uint32_stack_num_elems(struct uint32_stack *s)
{
    assert(s);
    return s->num_elems;
}

static inline bool
uint32_stack_is_empty(struct uint32_stack *s)
{
    assert(s);
    return uint32_stack_num_elems(s) == 0;
}


static inline uint32_t *
uint32_stack_peek_addr (struct uint32_stack *s)
{
    assert(s);
    if (!(s->num_elems)) {
        return (uint32_t *) 0x00;
    }
    return & (s->data[(s->num_elems) - 1]);
}


// UNSAFE !!!!!
static inline uint32_t *
uint32_stack_pop_addr (struct uint32_stack *s)
{
    assert(s);
    uint32_t *result = uint32_stack_peek_addr(s);
    if ( result ) {
        (s->num_elems)--;
    }
    return result;
}

static inline uint32_t
uint32_stack_pop (struct uint32_stack *s)
{
    assert(s);
    assert((s->num_elems));
    return s->data[--(s->num_elems)];
}

static inline void
uint32_stack_pop_discard (struct uint32_stack *s)
{
    assert(s);
    assert(s->num_elems);
    --(s->num_elems);
}


static inline bool
uint32_stack_enough_size_to_hold_n(struct uint32_stack *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->num_elems + n) <= (s->max_elems));
}
                 

void
uint32_stack_push( struct uint32_stack *s,
           uint32_t  elem )
{
    assert(elem);
    assert(s);
    if ( ! uint32_stack_enough_size_to_hold_n(s, 1)) {
        uint32_stack_grow(s);
    }
    s->data[s->num_elems] =  elem;
    s->num_elems ++;
}

static inline uint32_t *
uint32_stack_begin(struct uint32_stack *s)
{
    assert(s);
    return s->data;
}


static inline uint32_t *
uint32_stack_end(struct uint32_stack *s)
{
    assert(s);
    return & (s->data[s->num_elems]);
}




static inline uint32_t *
uint32_stack_next(uint32_t *prev)
{
    assert(prev);
    return (uint32_t*) ((uint8_t*) prev - sizeof(prev));
}



static inline uint32_t *
uint32_stack_prev(uint32_t *prev)
{
    assert(prev);
    return (uint32_t*) ((uint8_t*) prev + sizeof(prev));
}
