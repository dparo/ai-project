#ifndef TIMING_C_INCLUDE
#define TIMING_C_INCLUDE


//#######################################################
#endif /* TIMING_C_INCLUDE */
#if !defined TIMING_C_IMPLEMENTED && defined TIMING_C_IMPL
#define TIMING_C_IMPLEMENTED
//#######################################################


#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#else
#if _POSIX_C_SOURCE < 199309L
#error "In order to use the monotonic timer we need at least posix version 199309L"
#endif
#endif
#include <time.h>

    



#if 0
int clock_gettime(clockid_t clk_id, struct timespec *tp);

struct timespec {
    time_t   tv_sec;        /* seconds */
    long     tv_nsec;       /* nanoseconds */
};

#endif

struct timing {
    struct timespec tp;
};


struct timing
timing_diff(struct timing *start, struct timing *stop)
{
    struct timing result;
    
    if ((stop->tp.tv_nsec - start->tp.tv_nsec) < 0) {
        result.tp.tv_sec = stop->tp.tv_sec - start->tp.tv_sec - 1;
        result.tp.tv_nsec = stop->tp.tv_nsec - start->tp.tv_nsec + 1000000000UL;
    } else {
        result.tp.tv_sec = stop->tp.tv_sec - start->tp.tv_sec;
        result.tp.tv_nsec = stop->tp.tv_nsec - start->tp.tv_nsec;
    }

    return result;
}

struct timing
get_timing(void)
{
    struct timing t;
    int result = clock_gettime(CLOCK_MONOTONIC, &(t.tp));
    if (result != 0 ) {
        fatal("FATAL: Could not access the platform monotic timer");
    }
    assert(result == 0);
    return t;
}
    

void
timing_fprintf(FILE *f, struct timing *t)
{
    fprintf(f, "{ seconds: %zu, milli-seconds: %zu }", t->tp.tv_sec, t->tp.tv_nsec / 1000);
}





#endif /* TIMING_C_IMPL */
