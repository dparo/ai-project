/*
 * Copyright (C) 2018  Davide Paro
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
struct $(S) {
    $(T) *$(base);
    size_t $(num_elems);
    size_t $(max_elems);
};


struct $(S)
$(S)_create_sized(size_t $(num_elems))
{
    assert($(num_elems));
    struct $(S) result;
    size_t buf_size = $(num_elems) * sizeof($(T));
    result.$(base) = xmalloc(buf_size);
    assert(result.$(base));
    result.$(num_elems) = 0;
    result.$(max_elems) = $(num_elems);
    return result;
}


struct $(S)
$(S)_create(void)
{
    return $(S)_create_sized(64);
}


struct $(S)
$(S)_dup(struct $(S) *s)
{
    struct $S result;
    const size_t size = s->$max_elems * sizeof($T);
    result.$base = xmalloc(size);
    assert(result.$base);

    result.$num_elems = s->$num_elems;
    result.$max_elems = s->$max_elems;

    memcpy(result.$base, s->$base, s->$num_elems * sizeof($T));
    return result;
}

void
$(S)_free(struct $(S) *s)
{
    if ( s->$(base) ) {
        free(s->$(base));
    }
    s->$(base) = 0;
    s->$(num_elems) = 0;
    s->$(max_elems) = 0;
}

void
$(S)_clear(struct $(S) *s)
{
    $(S)_free(s);
}


void
$(S)_reset(struct $(S) *s)
{
    s->$(num_elems) = 0;
}



static inline void
$(S)_grow_to( struct $(S) *s,
              size_t new_$(max_elems))
{
    assert(s);
    if ( new_$(max_elems) == 0 ) {
        new_$(max_elems) = 64;
    }
    size_t new_size = new_$(max_elems) * sizeof($(T)) * 2;
    void *temp = xrealloc(s->$(base), new_size);
    assert(temp);
    s->$(base) = temp;
    s->$(max_elems) = new_$(max_elems);
}

static inline void
$(S)_grow(struct $(S) *s)
{
    assert(s);
    const size_t new_$(max_elems) = s->$(max_elems) * 2;
    $(S)_grow_to(s, new_$(max_elems));
}


static inline size_t
$(S)_$(num_elems)(struct $(S) *s)
{
    assert(s);
    return s->$(num_elems);
}

static inline bool
$(S)_is_empty(struct $(S) *s)
{
    assert(s);
    return $(S)_$(num_elems)(s) == 0;
}


static inline $T *
$(S)_peek_addr (struct $(S) *s)
{
    assert(s);
    if (!(s->$(num_elems))) {
        return ($T *) 0x00;
    }
    return & (s->$(base)[(s->$(num_elems)) - 1]);
}


// UNSAFE !!!!!
static inline $(T) *
$(S)_pop_addr (struct $(S) *s)
{
    assert(s);
    $(T) *result = $(S)_peek_addr(s);
    if ( result ) {
        (s->$(num_elems))--;
    }
    return result;
}

static inline $(T)
$(S)_pop (struct $(S) *s)
{
    assert(s);
    assert((s->$(num_elems)));
    return s->$(base)[--(s->$(num_elems))];
}

static inline void
$(S)_pop_discard (struct $(S) *s)
{
    assert(s);
    assert(s->$(num_elems));
    --(s->$(num_elems));
}


static inline bool
$(S)_enough_size_to_hold_n(struct $(S) *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->$(num_elems) + n) <= (s->$(max_elems)));
}
                 

void
$(S)_push( struct $(S) *s,
           $(T) $(ref) elem )
{
    assert(elem);
    assert(s);
    if ( ! $(S)_enough_size_to_hold_n(s, 1)) {
        $(S)_grow(s);
    }
    s->$(base)[s->$(num_elems)] = $(deref) elem;
    s->$(num_elems) ++;
}

static inline $(T) *
$(S)_begin(struct $(S) *s)
{
    assert(s);
    return s->$(base);
}


static inline $(T) *
$(S)_end(struct $(S) *s)
{
    assert(s);
    return & (s->$(base)[s->$(num_elems)]);
}




static inline $(T) *
$(S)_next($(T) *prev)
{
    assert(prev);
    return ($(T)*) ((uint8_t*) prev - sizeof(prev));
}



static inline $(T) *
$(S)_prev($(T) *prev)
{
    assert(prev);
    return ($(T)*) ((uint8_t*) prev + sizeof(prev));
}
