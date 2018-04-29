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


struct ast
ast_dup(struct ast *s)
{
    struct ast result;
    const size_t size = s->max_nodes * sizeof(struct ast_node);
    result.nodes = xmalloc(size);
    assert(result.nodes);

    result.num_nodes = s->num_nodes;
    result.max_nodes = s->max_nodes;

    memcpy(result.nodes, s->nodes, s->num_nodes * sizeof(struct ast_node));
    return result;
}

void
ast_free(struct ast *s)
{
    if ( s->nodes ) {
        free(s->nodes);
    }
    s->nodes = 0;
    s->num_nodes = 0;
    s->max_nodes = 0;
}

void
ast_clear(struct ast *s)
{
    ast_free(s);
}


void
ast_reset(struct ast *s)
{
    s->num_nodes = 0;
}



static inline void
ast_grow_to( struct ast *s,
              size_t new_max_nodes)
{
    assert(s);
    if ( new_max_nodes == 0 ) {
        new_max_nodes = 64;
    }
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


static inline size_t
ast_num_nodes(struct ast *s)
{
    assert(s);
    return s->num_nodes;
}

static inline bool
ast_is_empty(struct ast *s)
{
    assert(s);
    return ast_num_nodes(s) == 0;
}


static inline struct ast_node *
ast_peek_addr (struct ast *s)
{
    assert(s);
    if (!(s->num_nodes)) {
        return (struct ast_node *) 0x00;
    }
    return & (s->nodes[(s->num_nodes) - 1]);
}


// UNSAFE !!!!!
static inline struct ast_node *
ast_pop_addr (struct ast *s)
{
    assert(s);
    struct ast_node *result = ast_peek_addr(s);
    if ( result ) {
        (s->num_nodes)--;
    }
    return result;
}

static inline struct ast_node
ast_pop (struct ast *s)
{
    assert(s);
    assert((s->num_nodes));
    return s->nodes[--(s->num_nodes)];
}

static inline void
ast_pop_discard (struct ast *s)
{
    assert(s);
    assert(s->num_nodes);
    --(s->num_nodes);
}


static inline bool
ast_enough_size_to_hold_n(struct ast *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->num_nodes + n) <= (s->max_nodes));
}
                 

void
ast_push( struct ast *s,
           struct ast_node * elem )
{
    assert(elem);
    assert(s);
    if ( ! ast_enough_size_to_hold_n(s, 1)) {
        ast_grow(s);
    }
    s->nodes[s->num_nodes] = * elem;
    s->num_nodes ++;
}

static inline struct ast_node *
ast_begin(struct ast *s)
{
    assert(s);
    return s->nodes;
}


static inline struct ast_node *
ast_end(struct ast *s)
{
    assert(s);
    return & (s->nodes[s->num_nodes]);
}




static inline struct ast_node *
ast_next(struct ast_node *prev)
{
    assert(prev);
    return (struct ast_node*) ((uint8_t*) prev - sizeof(prev));
}



static inline struct ast_node *
ast_prev(struct ast_node *prev)
{
    assert(prev);
    return (struct ast_node*) ((uint8_t*) prev + sizeof(prev));
}
