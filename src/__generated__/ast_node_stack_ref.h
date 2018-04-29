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
struct ast_node_ref_stack {
    struct ast_node * *nodes;
    size_t num_nodes;
    size_t max_nodes;
};


struct ast_node_ref_stack
ast_node_ref_stack_create_sized(size_t num_nodes)
{
    assert(num_nodes);
    struct ast_node_ref_stack result;
    size_t buf_size = num_nodes * sizeof(struct ast_node *);
    result.nodes = xmalloc(buf_size);
    assert(result.nodes);
    result.num_nodes = 0;
    result.max_nodes = num_nodes;
    return result;
}


struct ast_node_ref_stack
ast_node_ref_stack_create(void)
{
    return ast_node_ref_stack_create_sized(64);
}


struct ast_node_ref_stack
ast_node_ref_stack_dup(struct ast_node_ref_stack *s)
{
    struct ast_node_ref_stack result;
    const size_t size = s->max_nodes * sizeof(struct ast_node *);
    result.nodes = xmalloc(size);
    assert(result.nodes);

    result.num_nodes = s->num_nodes;
    result.max_nodes = s->max_nodes;

    memcpy(result.nodes, s->nodes, s->num_nodes * sizeof(struct ast_node *));
    return result;
}

void
ast_node_ref_stack_free(struct ast_node_ref_stack *s)
{
    if ( s->nodes ) {
        free(s->nodes);
    }
    s->nodes = 0;
    s->num_nodes = 0;
    s->max_nodes = 0;
}

void
ast_node_ref_stack_clear(struct ast_node_ref_stack *s)
{
    ast_node_ref_stack_free(s);
}


void
ast_node_ref_stack_reset(struct ast_node_ref_stack *s)
{
    s->num_nodes = 0;
}



static inline void
ast_node_ref_stack_grow_to( struct ast_node_ref_stack *s,
              size_t new_max_nodes)
{
    assert(s);
    if ( new_max_nodes == 0 ) {
        new_max_nodes = 64;
    }
    size_t new_size = new_max_nodes * sizeof(struct ast_node *) * 2;
    void *temp = xrealloc(s->nodes, new_size);
    assert(temp);
    s->nodes = temp;
    s->max_nodes = new_max_nodes;
}

static inline void
ast_node_ref_stack_grow(struct ast_node_ref_stack *s)
{
    assert(s);
    const size_t new_max_nodes = s->max_nodes * 2;
    ast_node_ref_stack_grow_to(s, new_max_nodes);
}


static inline size_t
ast_node_ref_stack_num_nodes(struct ast_node_ref_stack *s)
{
    assert(s);
    return s->num_nodes;
}

static inline bool
ast_node_ref_stack_is_empty(struct ast_node_ref_stack *s)
{
    assert(s);
    return ast_node_ref_stack_num_nodes(s) == 0;
}


static inline struct ast_node * *
ast_node_ref_stack_peek_addr (struct ast_node_ref_stack *s)
{
    assert(s);
    if (!(s->num_nodes)) {
        return (struct ast_node * *) 0x00;
    }
    return & (s->nodes[(s->num_nodes) - 1]);
}


// UNSAFE !!!!!
static inline struct ast_node * *
ast_node_ref_stack_pop_addr (struct ast_node_ref_stack *s)
{
    assert(s);
    struct ast_node * *result = ast_node_ref_stack_peek_addr(s);
    if ( result ) {
        (s->num_nodes)--;
    }
    return result;
}

static inline struct ast_node *
ast_node_ref_stack_pop (struct ast_node_ref_stack *s)
{
    assert(s);
    assert((s->num_nodes));
    return s->nodes[--(s->num_nodes)];
}

static inline void
ast_node_ref_stack_pop_discard (struct ast_node_ref_stack *s)
{
    assert(s);
    assert(s->num_nodes);
    --(s->num_nodes);
}


static inline bool
ast_node_ref_stack_enough_size_to_hold_n(struct ast_node_ref_stack *s,
                           size_t n)
{
    assert(s);
    assert(n);
    return ((s->num_nodes + n) <= (s->max_nodes));
}
                 

void
ast_node_ref_stack_push( struct ast_node_ref_stack *s,
           struct ast_node * * elem )
{
    assert(elem);
    assert(s);
    if ( ! ast_node_ref_stack_enough_size_to_hold_n(s, 1)) {
        ast_node_ref_stack_grow(s);
    }
    s->nodes[s->num_nodes] = * elem;
    s->num_nodes ++;
}

static inline struct ast_node * *
ast_node_ref_stack_begin(struct ast_node_ref_stack *s)
{
    assert(s);
    return s->nodes;
}


static inline struct ast_node * *
ast_node_ref_stack_end(struct ast_node_ref_stack *s)
{
    assert(s);
    return & (s->nodes[s->num_nodes]);
}




static inline struct ast_node * *
ast_node_ref_stack_next(struct ast_node * *prev)
{
    assert(prev);
    return (struct ast_node **) ((uint8_t*) prev - sizeof(prev));
}



static inline struct ast_node * *
ast_node_ref_stack_prev(struct ast_node * *prev)
{
    assert(prev);
    return (struct ast_node **) ((uint8_t*) prev + sizeof(prev));
}
