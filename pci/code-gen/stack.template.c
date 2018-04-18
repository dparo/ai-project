struct $S {
    $T *base;
    size_t nelems;
    size_t buf_size;
};


struct $(S)
$(S)_init_sized(size_t buf_size)
{
}

struct $(S)
$(S)_init(void)
{
    $(S)_init_sized(sizeof(struct $(S)) * 64);
}

static inline void
$(S)_grow_to( struct $(S) *s,
              size_t newsize)
{
    void *temp = xrealloc(s->top, newsize);
    assert(temp);
    s->base = temp;
}

static inline void
$(S)_grow(struct $(S) *s)
{
    $(S)_grow_to(s, s->buf_size * 2);
}

static inline size_t
$(S)_num_elems(struct $(S) *s)
{
    return s->nelems;
}

static inline bool
$(S)_is_empty(struct $(S) *s)
{
    return $(S)_num_elems(s);
}


$(T) *
$(S)_peek_addr (struct $(S) *s)
{
    assert(s->nelems >= 0);
    return & (s->base[(s->nelems)]);
}


// UNSAFE !!!!!
$(T) *
$(S)_pop_addr (struct $(S) *s)
{
    $(T) *result = $(S)_peek_addr(s);
    (s->nelems)--;
    return result;
}

$(T)
$(S)_pop (struct $(S) *s)
{
    assert(s->nelems >= 0);
    return s->base[--(s->nelems)];
}

static inline $(T) *
$(S)_begin(struct $(S) *s)
{
    return s->base;
}

static inline $(T) *
$(S)_next($(T) *prev)
{
    return $(T) ((uint8_t*) prev + sizeof(prev));
}


$(T) *
$(S)_end(struct $(S) *s)
{
    return & (s->base[s->nelems]);
}
