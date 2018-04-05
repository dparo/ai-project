#ifndef CORO_C_INCLUDE
#define CORO_C_INCLUDE


//#######################################################
#endif /* CORO_C_INCLUDE */
#if !defined CORO_C_IMPLEMENTED && defined CORO_C_IMPL
#define CORO_C_IMPLEMENTED
//#######################################################


# define YIELD_NORES()                                                   \
    do { s.j = && jmp##__func__##__LINE__; return; jmp##__func__##__LINE__: {} } while(0)

# define YIELD(result)                                                  \
    do { s.j = && jmp##__func__##__LINE__; return (result); jmp##__func__##__LINE__: {} } while(0)


int
coro_enginnering(void)
{
    static struct {
        void *j;
        
        int a;
        int b;
    } s = {};

    if (s.j) {
        goto *(s.j);
    }
 
    
    memset(&s, 0, sizeof(s));

    s.a = 10;
    s.b = 11;


    YIELD(10);
       
    s.j = NULL;
    return 0;
    
}

int generator(void)
{
    static struct {
        void *j;

        
        int a;
    } s = {0};


    if (s.j) {
        goto *(s.j);
    } else {
        memset(&s, 0, sizeof(s));
    }



    for ( s.a = 0; s.a < 10; s.a++ ) {
        YIELD(s.a);
    }
    

    
    s.j = NULL;    //return 0;
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


bool
string_starts_with(char *starter,
                   char *string,
                   i32 string_len )
{
     bool result = true;
     i32 i;
     for ( i = 0;
           (starter[i] != 0) && (i < string_len);
           ++i ) {
          if ( starter[i] != string[i] ) {
               result = false;
               break;
          }
     }

     return result;
}







#endif /* CORO_C_IMPL */
