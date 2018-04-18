struct $(S) {
    $(T) *base;
    size_t num_elems;
    size_t max_elems;
};


struct $(S)
$(S)_init_sized(size_t num_elems)
{
    struct $(S) result;
    size_t buf_size = num_elems * sizeof($(T));
    result->base = xmalloc(buf_size);
    assert(result->base);
    result->num_elems = 0;
    result->max_elems = num_elems;
}


struct $(S)
$(S)_init(void)
{
    $(S)_init_sized(sizeof(struct $(S)) * 64);
}

static inline void
$(S)_grow_to( struct $(S) *s,
              size_t new_max_elems) /* Size in BYTES !!! */
{
    size_t new_size = new_max_elems * sizeof($(T)) * 2;
    void *temp = xrealloc(s->base, newsize);
    assert(temp);
    s->base = temp;
    s->max_elems = new_max_elems;
}

static inline void
$(S)_grow(struct $(S) *s)
{
    const size_t new_max_elems = s->max_elems * 2;
    $(S)_grow_to(s, new_size);
}

static inline size_t
$(S)_num_elems(struct $(S) *s)
{
    return s->num_elems;
}

static inline bool
$(S)_is_empty(struct $(S) *s)
{
    return $(S)_num_elems(s);
}


$(T) *
$(S)_peek_addr (struct $(S) *s)
{
    assert(s->num_elems >= 0);
    return & (s->base[(s->num_elems)]);
}


// UNSAFE !!!!!
$(T) *
$(S)_pop_addr (struct $(S) *s)
{
    $(T) *result = $(S)_peek_addr(s);
    (s->num_elems)--;
    return result;
}

$(T)
$(S)_pop (struct $(S) *s)
{
    assert(s->num_elems >= 0);
    return s->base[--(s->num_elems)];
}

static inline bool
$(S)_enough_size_to_hold_n(struct $(S) *s,
                           size_t n)
{
    return (& s->base[s->num_elems + n]) == & (s->base[s->max_elems]);
}
                 

void
$(S)_push( struct $(S) *s,
           $(T) *elem )
{
    if ( ! $(S)_enough_size_to_hold_n(s, 1)) {
        $(S)_grow(s);
    }
    s->base[s->num_elems] = *elem;
    s->num_elems ++;
}

static inline $(T) *
$(S)_begin(struct $(S) *s)
{
    return s->base;
}

static inline $(T) *
$(S)_next($(T) *prev)
{
    return ($(T)) ((uint8_t*) prev + sizeof(prev));
}


$(T) *
$(S)_end(struct $(S) *s)
{
    return & (s->base[s->num_elems]);
}
