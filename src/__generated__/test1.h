struct test1 {
    struct tms* *base;
    size_t num_elems;
    size_t max_elems;
};


struct test1
test1_create_sized(size_t num_elems)
{
    assert(num_elems);
    struct test1 result;
    size_t buf_size = num_elems * sizeof(struct tms*);
    result.base = xmalloc(buf_size);
    assert(result.base);
    result.num_elems = 0;
    result.max_elems = num_elems;
    return result;
}


struct test1
test1_create(void)
{
    return test1_create_sized(sizeof(struct test1) * 64);
}


void
test1_destroy(struct test1 *s)
{
    if ( s->base ) {
        free(s->base);
    }
    s->num_elems = 0;
    s->max_elems = 0;
}


static inline void
test1_grow_to( struct test1 *s,
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
test1_grow(struct test1 *s)
{
    assert(s);
    const size_t new_max_elems = s->max_elems * 2;
    test1_grow_to(s, new_max_elems);
}

static inline size_t
test1_num_elems(struct test1 *s)
{
    assert(s);
    return s->num_elems;
}

static inline bool
test1_is_empty(struct test1 *s)
{
    assert(s);
    return test1_num_elems(s);
}


struct tms* *
test1_peek_addr (struct test1 *s)
{
    assert(s);
    return & (s->base[(s->num_elems)]);
}


// UNSAFE !!!!!
struct tms* *
test1_pop_addr (struct test1 *s)
{
    assert(s);
    struct tms* *result = test1_peek_addr(s);
    (s->num_elems)--;
    return result;
}

struct tms*
test1_pop (struct test1 *s)
{
    assert(s);
    return s->base[--(s->num_elems)];
}

static inline bool
test1_enough_size_to_hold_n(struct test1 *s,
                           size_t n)
{
    assert(s);
    return (& s->base[s->num_elems + n]) == & (s->base[s->max_elems]);
}
                 

void
test1_push( struct test1 *s,
           struct tms* *elem )
{
    assert(elem);
    assert(s);
    if ( ! test1_enough_size_to_hold_n(s, 1)) {
        test1_grow(s);
    }
    s->base[s->num_elems] = *elem;
    s->num_elems ++;
}

static inline struct tms* *
test1_begin(struct test1 *s)
{
    assert(s);
    return s->base;
}


struct tms* *
test1_end(struct test1 *s)
{
    assert(s);
    return & (s->base[s->num_elems]);
}




static inline struct tms* *
test1_next(struct tms* *prev)
{
    assert(prev);
    return (struct tms**) ((uint8_t*) prev - sizeof(prev));
}



static inline struct tms* *
test1_prev(struct tms* *prev)
{
    assert(prev);
    return (struct tms**) ((uint8_t*) prev + sizeof(prev));
}
