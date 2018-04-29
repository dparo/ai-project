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
#ifndef PLATFORM_C_INCLUDE
#define PLATFORM_C_INCLUDE


#include "utils.c"
#include <string.h>



#define WINDOWS _WIN16 || _WIN32 || _WIN64


typedef enum Pages_Prot_Flags {
    PAGE_PROT_NONE = 0,
    PAGE_PROT_READ = (1 << 0),
    PAGE_PROT_WRITE = (1 << 2),
    PAGE_PROT_EXEC  = (1 << 3),
} Page_Prot_Flags;



typedef enum Page_Type_Flags {
    PAGE_FIXED = 0,
    PAGE_SHARED = (1 << 1),
    PAGE_PRIVATE = (1 << 2),
    PAGE_ANONYMOUS = (1 << 3),
    PAGE_LOCKED = (1 << 4),
    PAGE_GROWSDOWN = (1 << 5),
    PAGE_POPULATE = (1 << 6),
    PAGE_HUGETLB = (1 << 7),
    PAGE_HUGE_2MB = (1 << 8),
    PAGE_HUGE_1GB = (1 << 9),
} Page_Type_Flags;

typedef enum Page_Remap_Flags {
    PAGE_REMAP_NONE = 0,
    PAGE_REMAP_MAYMOVE = (1 << 0),
    PAGE_REMAP_FIXED = (1 << 1),
} Page_Remap_Flags;


typedef struct Platform {
    i64 page_size;
} Platform;

extern Platform platform;


// #######################################################################
#endif /* PLATFORM_C_INCLUDE */
#if !defined PLATFORM_C_IMPLEMENTED && defined PLATFORM_C_IMPL
#define PLATFORM_C_IMPLEMENTED
// #######################################################################


Platform platform;


#if __linux__
#  include <errno.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <signal.h>
#  include <sys/wait.h>
#  define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)
#  define MAP_HUGE_1GB    (30 << MAP_HUGE_SHIFT)
#elif WINDOWS
#  include <windows.h>
#endif

#if __linux__


typedef int File_Handle;
typedef pid_t Proc_Handle;

static const File_Handle Invalid_File_Handle = -1;
static const Proc_Handle Invalid_Proc_Handle = -1;


#else
typedef HFILE File_Handle;
typedef int Proc_Handle;
static const File_Handle Invalid_File_Handle = HFILE_ERROR;

#endif



#ifdef __linux__
#include <execinfo.h>
#include <stdlib.h>

void
linux_print_stack_trace(void)
{
    void *array[32];
    size_t size;

    fprintf(stderr, "Printing stack trace: #####\n");
    // get void*'s for all entries on the stack
    size = backtrace(array, ARRAY_LEN(array));
    // print out all the frames to stderr

    backtrace_symbols_fd(array, size, STDERR_FILENO);
    fprintf(stderr, "########################\n");
    fflush(stderr);
}


#if defined __linux__ && defined __DEBUG

void gdb_print_trace(void) {
    char pid_buf[30];
    snprintf(pid_buf, sizeof(pid_buf), "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
    int child_pid = fork();
    if (!child_pid) {
        dup2(2,1); // redirect output to stderr
        fprintf(stdout,"stack trace for %s pid=%s\n",name_buf,pid_buf);
        execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt full", name_buf, pid_buf, NULL);
        abort(); /* If gdb failed to start */
    } else {
        waitpid(child_pid,NULL,0);
    }
}

#endif

void sigsegv_linux_backtrace(int sig)
{
    fprintf(stderr, "Error: signal %d: %s\n", sig, strsignal(sig));
    LOG_STACK_TRACE();
    exit(-1);
}
#endif


i64
platform_get_page_size( void )
{
#if __linux__
    assert_msg(platform.page_size, "Can't get valid page_size, did you call platform_init at the start of main?");
    return platform.page_size;
#elif WINDOWS
#    error "Needs implementation for Windows"
#else
#    error "Not supported platform"
#endif
}


#ifdef __linux__
typedef enum File_Open_Flags {
    FILE_NONE = 0,
    FILE_EXEC = (1 << 0),
    FILE_RDONLY = (1 << 1),
    FILE_WRONLY = (1 << 2),
    FILE_RDWR = (1 << 3),
    FILE_SEARCH = (1 << 4),
    FILE_APPEND = (1 << 5),
    FILE_CREAT = (1 << 6),
    FILE_DIRECTORY = (1 << 7),
    FILE_DSYNC = (1 << 8),
    FILE_EXCL = (1 << 9),
    FILE_NOCTTY = (1 << 10),
    FILE_NOFOLLOW = (1 << 11),
    FILE_NONBLOCK = (1 << 12),
    FILE_RSYNC = (1 << 13),
    FILE_SYNC = (1 << 14),
    FILE_TRUNC = (1 << 15),
} File_Open_Flags;


#else
#   error "Needs implementation"
#endif


File_Handle
file_open(char *path, File_Open_Flags flags)
{
#ifdef __linux__
    {
        int oflag = 0; {
            if (flags & FILE_EXEC) {
                debug_break();
                // oflag |= O_EXEC;
            }
            if (flags & FILE_RDONLY) {
                oflag |= O_RDONLY;
            }
            if (flags & FILE_WRONLY) {
                oflag |= O_WRONLY;
            }
            if (flags & FILE_RDWR) {
                oflag |= O_RDWR;
            }
            if (flags & FILE_SEARCH) {
                debug_break();
                // missing symbol O_SEARCJ wtf!!
                // oflag |= O_SEARCH;
            }
            if (flags & FILE_APPEND) {
                oflag |= O_APPEND;
            }
            if (flags & FILE_CREAT) {
                oflag |= O_CREAT;
            }
            if (flags & FILE_DIRECTORY) {
                oflag |= O_DIRECTORY;
            }
            if (flags & FILE_DSYNC) {
                oflag |= O_DSYNC;
            }
            if (flags & FILE_EXCL) {
                oflag |= O_EXCL;
            }
            if (flags & FILE_NOCTTY) {
                debug_break();
                oflag |= O_NOCTTY;
            }
            if (flags & FILE_NOFOLLOW) {
                oflag |= O_NOFOLLOW;
            }
            if (flags & FILE_NONBLOCK) {
                oflag |= O_NONBLOCK;
            }
            if (flags & FILE_RSYNC) {
                oflag |= O_RSYNC;
            }
            if (flags & FILE_SYNC) {
                oflag |= O_SYNC;
            }
            if (flags & FILE_TRUNC) {
                oflag |= O_TRUNC;
            }

        }
        File_Handle result = open(path, oflag, S_IXUSR | S_IXGRP |S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        assert(result != Invalid_File_Handle);
        return result;
    }
#else
#    error "Needs implementation"
#endif

}
int
file_close(File_Handle fh)
{
#ifdef __linux__
    {
        // returns -1 on failure
        return close(fh);
    }
#else
#    error "Needs implementation"
#endif

}





// Insipired by mmap function
void*
alloc_memmapped_aux( void* addr, size_t size, Page_Prot_Flags prot, Page_Type_Flags type, File_Handle fh)
{
#if __linux__
    void *result = 0x0000;
    int mprot = 0; {
        if ( prot & PAGE_PROT_READ ) {
            mprot |= PROT_READ;
        }
        if (prot & PAGE_PROT_WRITE ) {
            mprot |= PROT_WRITE;
        }
        if (prot & PAGE_PROT_EXEC ) {
            mprot |= PROT_EXEC;
        }

    }
    int mflags = 0; {
        if ( type & PAGE_FIXED ) {
            mflags |= MAP_FIXED;
        }
        if ( type & PAGE_PRIVATE ) {
            mflags |= MAP_PRIVATE;
        }
        if ( type & PAGE_ANONYMOUS ) {
            mflags |= MAP_ANONYMOUS;
        }
        if ( type & PAGE_LOCKED ) {
            mflags |= MAP_LOCKED;
        }
        if ( type & PAGE_GROWSDOWN ) {
            mflags |= MAP_GROWSDOWN;
        }
        if ( type & PAGE_POPULATE ) {
            mflags |= MAP_POPULATE;
        }
        if ( type & PAGE_HUGETLB ) {
            mflags |= MAP_HUGETLB;
        }
        if ( type & PAGE_HUGE_2MB ) {
            mflags |= MAP_HUGE_2MB;
        }
        if ( type & PAGE_HUGE_1GB ) {
            mflags |= MAP_HUGE_1GB;
        }
    }
    i32 offset = 0;
    result = mmap( addr, size, mprot, mflags, fh, offset);
    if (result == MAP_FAILED ) {
        int errno_val = errno;
        char *error_message = strerror(errno_val);
        assert(result != MAP_FAILED);
    }
    return result;
#elif WINDOWS
#    error "Needs windows implementation"
#else
#    error "What the fuck"
#endif
}

void*
alloc_memmapped_pages( void* addr, size_t size, Page_Prot_Flags prot, Page_Type_Flags type )
{
    const File_Handle fh = 0;
    return alloc_memmapped_aux( addr, size, prot, type, fh );
}


// new addr can be NULL if you don't care where its going to be remapped
void*
realloc_memmapped_pages( void* old_addr, size_t old_size, void *new_addr, size_t new_size,
                         Page_Remap_Flags flags )
{
#ifdef __linux__
    {
        void *result = NULL;
        int mflags = 0; {
            if ( flags & PAGE_REMAP_MAYMOVE ) {
                mflags |= MREMAP_MAYMOVE;
            }
            if ( flags & PAGE_REMAP_FIXED ) {
                mflags |= MREMAP_FIXED;
            }
        }
        if ( new_addr ) {
            result = mremap(old_addr, old_size, new_size, mflags, new_addr);
        }
        else {
            result = mremap(old_addr, old_size, new_size, mflags );
        }
        assert(result);
        return result;
    }
#else
#error "Needs implementation for current platform"
#endif
}




// addr is usually null
void*
alloc_memmapped_file(char *file, void* addr, Page_Prot_Flags prot, Page_Type_Flags type,
                     bool zeroed_page_before, size_t appended_zeroes,
                     i64  __OUT__ *buffer_len )
{
#if __linux__
    void *result = 0x0000;
    File_Handle fh = file_open(file, FILE_RDWR);  // TODO: Support passing other params here?
    assert(fh != 1);
    struct stat fdstat;
    int st = stat(file, &fdstat);
    assert(st == 0);
    i64 page_size = platform_get_page_size();
    assert(platform.page_size);
    u8 *zeroed_page = 0x0000;

    if ( zeroed_page_before ) {
        zeroed_page = alloc_memmapped_aux( addr, page_size + fdstat.st_size + appended_zeroes,
                                           prot, type, 0 );
        assert(zeroed_page);
    }
    void *newaddr = addr;
    Page_Type_Flags ptype = type;
    ptype = ptype & (~PAGE_ANONYMOUS);
    if ( zeroed_page_before ) {
        newaddr = zeroed_page + page_size;
        ptype |= PAGE_FIXED;
    }
    result = alloc_memmapped_aux ( newaddr, fdstat.st_size + appended_zeroes,
                                   prot, ptype, fh );

    assert(result);
    if ( buffer_len ) {
        *buffer_len = fdstat.st_size + appended_zeroes;
    }

    file_close(fh);
    return result;
#endif
}



int
dealloc_memmapped( void* addr, size_t size )
{
#if __linux__
    int result = 0; // 0 = success
    result = munmap(addr, size);
    return result;
#else
#error "Needs implementation"
#endif
}




typedef void* Alloc_Frame_Handle;


typedef struct Stack_Allocator {
#define STACK_ALLOCATOR_MAX_NESTED_FRAMES 16
    ptr buffer;
    size_t buffer_size;
    ptr sp;

    size_t states_it;
    Alloc_Frame_Handle states[STACK_ALLOCATOR_MAX_NESTED_FRAMES];
} Stack_Allocator;

static Stack_Allocator temp_storage = {0};


void
stack_allocator_create( Stack_Allocator *result, size_t initial_size )
{
    assert(initial_size);
    size_t page_size = platform_get_page_size();
    if ( initial_size < page_size ) {
        initial_size = page_size;
    }

    // clamped and sanitiezed allocation size on page basis
    size_t alloc_size = (initial_size + page_size - 1) & (~(page_size - 1));
    result->buffer = alloc_memmapped_pages(0, alloc_size, PAGE_PROT_READ | PAGE_PROT_WRITE,
                                           PAGE_PRIVATE | PAGE_ANONYMOUS);
    result->buffer_size = alloc_size;
    assert ( result->buffer );
    result->sp = result->buffer;
    result->states_it = 0;

    zero_struct(result->states);
}

void
stack_allocator_destroy(Stack_Allocator *all )
{
    assert(all->buffer);
    dealloc_memmapped(all->buffer, all->buffer_size);

    zero_struct(all);
}

static inline void
_stack_allocator_push_state(Stack_Allocator *all, Alloc_Frame_Handle *handle)
{
    assert(all->states_it < STACK_ALLOCATOR_MAX_NESTED_FRAMES);
    all->states[all->states_it] = handle;
    ++(all->states_it);
}


void*
stack_allocator_push_sized(Stack_Allocator *all, void *data, bool clear, size_t size)
{
    void *result = NULL;
    assert(all->buffer);
    assert(all->sp >= all->buffer);
    assert(all->buffer_size);

    if (all->sp + size > all->buffer + all->buffer_size ) {
        assert(0);
    }

    result = all->sp;
    if ( data ) {
        memcpy(all->sp, data, size);
    }
    else {
        if ( clear == true ) {
            memclr(all->sp, size);
        }
    }


    all->sp += size;

    return result;
}


void
stack_allocator_pop_sized( Stack_Allocator *all, void *dest, size_t size )
{
    void *result = NULL;
    assert(all->buffer);
    assert(all->sp >= all->buffer);
    assert(all->buffer_size);

    assert(all->sp - size >= all->buffer);

    all->sp -= size;
    if ( dest ) {
        memcpy(dest, all->sp, size);
    }

}




// align = alignment of struct/data types to push later
// size = already the size that u want to push in the allocator, NOTE you can always push_later
//        with stack_allocator_push_xxx(...)
Alloc_Frame_Handle
stack_allocator_new_frame(Stack_Allocator *all, size_t align, size_t size, bool clear)
{
    Alloc_Frame_Handle result = NULL;
    assert(align <= (size_t) platform_get_page_size());

    // align must be on 64 bytes boundaries to allow better cache lines coherence
    align = (align + 64) & ~(64 - 1);

    if (!all->buffer || !all->sp) {
        assert(all->sp == NULL && all->buffer == NULL); // if buffer is NULL, sp should be too
        stack_allocator_create(all, platform_get_page_size());
        _stack_allocator_push_state(all, (Alloc_Frame_Handle) all->sp);

        result = all->buffer;
    }
    else {
        assert(all->sp);
        assert(all->buffer_size);

        Alloc_Frame_Handle prev_sp = all->sp;
        size_t base = (size_t) all->buffer;
        size_t sp = (size_t) all->sp;

        sp = base + (((align + (sp - base) - 1) / align) * align);
        all->sp = (void*) sp;

        assert(all->states_it < STACK_ALLOCATOR_MAX_NESTED_FRAMES);
        _stack_allocator_push_state(all, (Alloc_Frame_Handle) all->sp);

        result = prev_sp;
    }
    if (size) {
        stack_allocator_push_sized(all, NULL, clear, size);
    }
    return result;

}



#define stack_allocator_push(ALL, DATA)                     \
    stack_allocator_push_sized((ALL), (DATA), sizeof(DATA))

#define stack_allocator_pop(ALL, DEST)                        \
    stack_allocator_pop_sized((ALL), (DEST), sizeof(*(DEST)))


#define stack_allocator_request(ALL, SIZE)          \
    stack_allocator_push_sized((ALL), NULL, (SIZE))




// @TODO: Maybe the frame deletion should reclaim some memory if needed?
// we need the handle to make an extra check to make sure that the frame
// gets deleted from the proper owner that holds the right handle
inline static void
stack_allocator_del_frame(Stack_Allocator *all, Alloc_Frame_Handle handle)
{
    (void) handle;

    assert(all->states_it != 0); // Nothing to pop

    --(all->states_it);

    assert(all->states[all->states_it] == handle);

    size_t shrink_amount = (size_t) all->sp - (size_t) all->states[all->states_it];

    all->sp = all->states[all->states_it];

    // Reset the states
    all->states[all->states_it] = 0;
}

static inline Alloc_Frame_Handle
temp_storage_new_frame(size_t align, size_t size, bool clear)
{
    return stack_allocator_new_frame(& temp_storage, align, size, clear);
}

inline static void
temp_storage_del_frame(Alloc_Frame_Handle handle)
{
    stack_allocator_del_frame(& temp_storage, handle);
}


void*
temp_storage_push_sized(void* data, bool clear, size_t size)
{
    return stack_allocator_push_sized(& temp_storage, data, clear, size);
}

void
temp_storage_pop_sized( void *dest, size_t size )
{
    stack_allocator_pop_sized( & temp_storage, dest, size );
}


#define temp_storage_push(DATA)                   \
    temp_storage_push_sized((DATA), sizeof(DATA))

#define temp_storage_pop(DEST)                      \
    temp_storage_pop_sized((DEST), sizeof(*(DEST)))


#define temp_storage_request(SIZE)              \
    temp_storage_push_sized(NULL, (SIZE))




#ifdef __linux__
// #########################################################
// #####
// ##### Linux stuff
// #####





// #####
// ##### End of Linux stuff
// #####
// #########################################################
#elif WINDOWS
#    error "Error"
#else
#    errror "Error"
#endif




int
platform_init( void )
{
#ifdef __linux__
    {
        int result = false;

        { // Page Size retrieve
            long page_size = sysconf(_SC_PAGESIZE);
            assert( page_size );
            platform.page_size = page_size;
            signal(SIGSEGV, sigsegv_linux_backtrace);   // install our handler
        }

        {
            stack_allocator_create( & temp_storage, 1 * 1024 * 1024);
        }

        return result;
    }
#elif WINDOWS
#    error "Implement me for Windows please"
#else
#    error "Not supported platform"
#endif
}

int
create_directory(char *path )
{
#ifdef __linux__
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
#   error "Not supported platform"
#endif
}



i64
file_read(File_Handle file, void *buf, i64 size_to_read)
{
#ifdef __linux__
    {
        i64 result = read(file, buf, size_to_read);

        if (result < 0) {
            char *err_message = strerror(errno);
            debug_break();
        }
        return result;
    }
#else
#    error "Not supported platform, needs implementation"
#endif

}


i64
file_write(File_Handle file, void *buf, i64 size_to_write)
{
#ifdef __linux__
    {
        i64 result = write(file, buf, size_to_write);
        if (result == -1 || (result != size_to_write) ) {
            char *err_message = strerror(errno);
            debug_break();
        }
        return result;
    }
#else
#    error "Not supported platform needs implementation"
#endif

}



Proc_Handle
proc_spawn_sync ( char *command, int * exit_status )
{
#ifdef __linux__
    pid_t id = fork ();
    if ( id == - 1) {
        // error on fork
    }
    else if ( id == 0 ) {
        // child process
        if (execl("/bin/sh", "sh", "-c", command, (char *) 0) ) {
            // DOH Make me crash
            *((char*)0) = 0;
        }
    }
    else  {
        // parent process
        // wait for child to complete
        waitpid(-1, exit_status, 0);
    }
    return id;
#elif WINDOWS
# error "Not supported platform needs implementation"
#else
# error "Not supported platform needs implementation"
#endif
}





Proc_Handle
proc_spawn_async ( char * command )
{
#ifdef __linux__
    pid_t id = fork ();
    if ( id == - 1) {
        // error on fork
    }
    else if ( id == 0 ) {
        // child process
        if ( execl("/bin/sh", "sh", "-c", command, (char *) 0) ) {
            // DOH Make me crash

            *((char*)0) = 0;
        }
    }
    else  {
        // parent process
    }
    return id;
#else
#    error "Not supported platform needs implementation"
#endif
}


Proc_Handle
proc_spawn_async_piped ( char *command,
                         File_Handle *inpipe, File_Handle *outpipe)
{
#ifdef __linux__
    File_Handle outpipes[2] = {0};
    File_Handle inpipes[2] = {0};
    int piperes = pipe(outpipes);
    assert(piperes == 0);
    piperes = pipe(inpipes);
    assert(piperes == 0 );
    pid_t pid = fork ();
    if ( pid == -1) {
        // error on fork
        assert_msg(0, "Failed to fork the requested proc");
    }
    else if ( pid == 0 ) {
        // child process
        if (inpipe) {
            dup2(inpipes[0], STDIN_FILENO);
        }
        close(inpipes[0]);
        close(inpipes[1]);
        if (outpipe) {
            dup2(outpipes[1], STDOUT_FILENO);
        }
        close(outpipes[0]);
        close(outpipes[1]);
        if (execl("/bin/bash", "bash", "-c", command, (char *) 0) ) {
            // DOH Make me crash
            *((char*)0) = 0;
        }
    }
    else  {
        close(inpipes[0]);
        close(outpipes[1]);
        if (inpipe) {
            *inpipe = inpipes[1];
        } else {
            close(inpipes[1]);
        }
        if (outpipe) {
            *outpipe = outpipes[0];
        } else {
            close(outpipes[0]);
        }
    }
    return pid;
#elif WINDOWS
# error  "Not supported platform needs implementation"
#else
# error "Not supported platform needs implementation"
#endif
}


void
proc_sync ( Proc_Handle proc, int *status )
{
#ifdef __linux__
    int local_status;
    waitpid ( proc, (status != NULL) ? status : &local_status, 0 );
#else
#   error "Not supported platform needs implementation"
#endif
}


#endif /* PLATFORM_C_IMPL */
