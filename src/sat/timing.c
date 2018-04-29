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
int clock_gettime(clockid_t clk_id, struct timespec *ts);

struct timespec {
    time_t   tv_sec;        /* seconds */
    long     tv_nsec;       /* nanoseconds */
};

#endif

struct timing {
    struct timespec ts;
};


struct timing
timing_diff(struct timing *start, struct timing *stop)
{
    struct timing result;
    
    if (stop->ts.tv_nsec < start->ts.tv_nsec) {
        result.ts.tv_sec = stop->ts.tv_sec - start->ts.tv_sec - 1;
        result.ts.tv_nsec = stop->ts.tv_nsec - start->ts.tv_nsec + 1000000000UL;
    } else {
        result.ts.tv_sec = stop->ts.tv_sec - start->ts.tv_sec;
        result.ts.tv_nsec = stop->ts.tv_nsec - start->ts.tv_nsec;
    }

    return result;
}

struct timing
get_timing(void)
{
    struct timing t;
    int result = clock_gettime(CLOCK_MONOTONIC, &(t.ts));
    if (result != 0 ) {
        fatal("FATAL: Could not access the platform monotic timer");
    }
    assert(result == 0);
    return t;
}
    

void
timing_fprintf(FILE *f, struct timing *t)
{
    fprintf(f, "{ seconds: %zu, milli-seconds: %zu }", t->ts.tv_sec, t->ts.tv_nsec / 1000 / 1000);
}





#endif /* TIMING_C_IMPL */
