struct test2 {
    struct tms* *base;
    size_t num_elems;
    size_t max_elems;
};


struct test2
test2_create_sized(size_t num_elems)
{
    assert(num_elems);
    struct test2 result;
    size_t buf_size = num_elems * sizeof(struct tms*);
    result.base = xmalloc(buf_size);
    assert(result.base);
    result.num_elems = 0;
    result.max_elems = num_elems;
    return result;
}


struct test2
test2_create(void)
{
    return test2_create_sized(sizeof(struct test2) * 64);
}


void
test2_destroy(struct test2 *s)
{
    if ( s->base ) {
        free(s->base);
    }
    s->num_elems = 0;
    s->max_elems = 0;
}


static inline void
test2_grow_to( struct test2 *s,
              size_t new_max_elems) /* Size in BYTES !!! */
{
    assert(s);
    size_t new_size = new_max_elems * sizeof(struct tms*) * 2;
    void *temp = xrealloc(s->base, new_size);
    assert(temp);
    s->base = temp;
    s->max_elems = new_max_elems;
}

static inline void
test2_grow(struct test2 *s)
{
    assert(s);
    const size_t new_max_elems = s->max_elems * 2;
    test2_grow_to(s, new_max_elems);
}

static inline size_t
test2_num_elems(struct test2 *s)
{
    assert(s);
    return s->num_elems;
}

static inline bool
test2_is_empty(struct test2 *s)
{
    assert(s);
    return test2_num_elems(s);
}


struct tms* *
test2_peek_addr (struct test2 *s)
{
    assert(s);
    return & (s->base[(s->num_elems)]);
}


// UNSAFE !!!!!
struct tms* *
test2_pop_addr (struct test2 *s)
{
    assert(s);
    struct tms* *result = test2_peek_addr(s);
    (s->num_elems)--;
    return result;
}

struct tms*
test2_pop (struct test2 *s)
{
    assert(s);
    return s->base[--(s->num_elems)];
}

static inline bool
test2_enough_size_to_hold_n(struct test2 *s,
                           size_t n)
{
    assert(s);
    return (& s->base[s->num_elems + n]) == & (s->base[s->max_elems]);
}
                 

void
test2_push( struct test2 *s,
           struct tms* *elem )
{
    assert(elem);
    assert(s);
    if ( ! test2_enough_size_to_hold_n(s, 1)) {
        test2_grow(s);
    }
    s->base[s->num_elems] = *elem;
    s->num_elems ++;
}

static inline struct tms* *
test2_begin(struct test2 *s)
{
    assert(s);
    return s->base;
}


struct tms* *
test2_end(struct test2 *s)
{
    assert(s);
    return & (s->base[s->num_elems]);
}




static inline struct tms* *
test2_next(struct tms* *prev)
{
    assert(prev);
    return (struct tms**) ((uint8_t*) prev - sizeof(prev));
}



static inline struct tms* *
test2_prev(struct tms* *prev)
{
    assert(prev);
    return (struct tms**) ((uint8_t*) prev + sizeof(prev));
}
