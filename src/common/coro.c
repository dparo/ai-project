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
#ifndef CORO_C_INCLUDE
#define CORO_C_INCLUDE


//#######################################################
#endif /* CORO_C_INCLUDE */
#if !defined CORO_C_IMPLEMENTED && defined CORO_C_IMPL
#define CORO_C_IMPLEMENTED
//#######################################################


# define YIELD_NORES()                                                   \
    do { s.__coro__jmp__ = && jmp##__func__##__LINE__; return; jmp##__func__##__LINE__: {} } while(0)

# define YIELD(result)                                                  \
    do { s.__coro__jmp__ = && jmp##__func__##__LINE__; return (result); jmp##__func__##__LINE__: {} } while(0)




int
coro_enginnering(void)
{
    static struct {
        void *__coro__jmp__;
        
        int a;
        int b;
    } s = {};

    if (s.__coro__jmp__) {
        goto *(s.__coro__jmp__);
    }
 
    
    memset(&s, 0, sizeof(s));

    s.a = 10;
    s.b = 11;


    YIELD(10);
       
    s.__coro__jmp__ = NULL;
    return 0;
    
}



int generator(void)
{
    static struct {
        void *__coro__jmp__;

        
        int a;
    } s = {0};


    if (s.__coro__jmp__) {
        goto *(s.__coro__jmp__);
    } else {
        memset(&s, 0, sizeof(s));
    }



    for ( s.a = 0; s.a < 10; s.a++ ) {
        YIELD(s.a);
    }
    

    
    s.__coro__jmp__ = NULL;    //return 0;
    return -1;

}



void test_generator(void)
{
    int value = 0;
    for ( int it = 0; it < 3; it++ ) {
        while (value = generator(), value != -1 ) {
            printf("iteration %d -> %d\n", it, value);
        }
    }
}



#define CORO_BEGIN(s, ...)                             \
    static struct {                                    \
        void *__coro__jmp__;                           \
        __VA_ARGS__                                    \
    } (s) = {0};                                       \
    if ((s).__coro__jmp__) {                           \
        goto *((s).__coro__jmp__);                     \
    } else { memset( &(s), 0, sizeof(s)); }
    

#define CORO_END(s)                          \
    (s).__coro__jmp__ = (void*) 0;             

#define CORO_END_VAL(s, x)                   \
    (s).__coro__jmp__ = (void*) 0;              \
    return x;




enum condition {
    condition_finished,
    condition_need_more_memory,
};

enum condition
test(void)
{
    CORO_BEGIN
        (s, int it; int a; int b; int c;); {


        for ( s.it = 0; s.it < 100; (s.it)++) {
            YIELD(condition_need_more_memory);
            assert(enough_memory());
            assert(enough_memory());
        }

    } CORO_END(s);
    return condition_finished;
}





#endif /* CORO_C_IMPL */
