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
#ifndef LISTS_C_INCLUDE
#define LISTS_C_INCLUDE


//#######################################################
#endif /* LISTS_C_INCLUDE */
#if !defined LISTS_C_IMPLEMENTED && defined LISTS_C_IMPL
#define LISTS_C_IMPLEMENTED
//#######################################################


/* 
   // list definition
   l = [a, b, c, e];
   l = ;

   what is a list?
   it's a hashmap of symbols

   list-table
   it's a hashmap of list-names/symbols which gives
   the hash-map for that symbol
 */


#define LIST_T_MAX_NAME_LEN 256
typedef struct list_t {
    char name[LIST_T_MAX_NAME_LEN];
    // The dictionary contains unpositional literals inside the list
    stb_sdict *dict;
} list_t;


list_t
list_create(char *name, size_t name_len)
{

    // @TODO: Add this list to the list talbe, push into some stack, and make it indexed with a hashmap
    
    list_t list;
    list.dict = stb_sdict_new(1);
    strncpy(list.name, name, MIN(name_len, LIST_T_MAX_NAME_LEN));
    return list;
}

void
list_free(list_t *list)
{
    assert(list);
    stb_sdict_delete(list->dict);
    list->dict = NULL;
}


void
list_add_literal(list_t *list,
                 struct ast_node *node)
{
    assert(list);
    assert(list->dict);

    assert(node->type == AST_NODE_TYPE_IDENTIFIER);

    // Not already inside the symtable

    char *string = node->text;
    size_t string_len = node->text_len;
    assert(string);
    assert(string_len);
    // null terminate before adding
    char temp = string[string_len];
    string[string_len] = 0;

#warning @TODO: Need to check presence of same literal inside the list

    void *value = (void*) 0x1;
    stb_sdict_set(list->dict, string, value);

    // restore null termination
    string[string_len] = temp;
}


#endif /* LISTS_C_IMPL */
