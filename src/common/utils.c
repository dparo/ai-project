#ifndef UTILS_C_INCLUDE
#define UTILS_C_INCLUDE


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int32_t i32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;
typedef u8* ptr;
typedef float f32;
typedef double f64;

#ifndef NULL
#define NULL ((void*) 0x0)
#endif


#define cast(T, expr) (T) (expr)



#if defined  __GNUC__ || defined __GNUG__ || defined __clang__
#    define ARRAY_LEN(arr) \
        (sizeof(arr) / sizeof((arr)[0]) \
         + sizeof(typeof(int[1 - 2 * \
               !!__builtin_types_compatible_p(typeof(arr), \
                     typeof(&arr[0]))])) * 0)
#else
#    define ARRAY_LEN(A)                            \
          (sizeof(A) / sizeof((A)[0]))
#endif



#define STRINGIFY(x) #x
#define __AT_SRC__ __FILE__ ":" STRINGIFY(__LINE__)

#define CONCAT_(a, ...) a ## __VA_ARGS__
#define CONCAT(a, ...) CONCAT_(a, __VA_ARGS__)

/* Example of deferring */
/* #define A() 123 */
/* A() // Expands to 123 */
/* DEFER(A)() // Expands to A () because it requires one more scan to fully expand */
/* EXPAND(DEFER(A)()) // Expands to 123, because the EXPAND macro forces another scan */

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__




// Supports for functions that gets run before entering in main
// example CONSTRUCT(my_init)
// void my_init(void) { ... }
#ifdef __GNUC__
#  define CONSTRUCT(_func) static void _func (void) __attribute__((constructor));
#  define DESTRUCT(_func) static void _func (void) __attribute__((destructor));
#  define DEPRECATED __attribute__((deprecated))
#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
#define CONSTRUCT(_func)                                                \
    static void _func(void);                                            \
    static int _func ## _wrapper(void) { _func(); return 0; }           \
    __pragma(section(".CRT$XCU",read))                                  \
    __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _wrapper;

#define DESTRUCT(_func)                                                 \
    static void _func(void);                                            \
    static int _func ## _constructor(void) { atexit (_func); return 0; } \
    __pragma(section(".CRT$XCU",read))                                  \
    __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#else
#error "You will need constructor support for your compiler"
#endif
// #################################################


#ifdef __GNUC__
#   define PRINTF_STYLE(STRING_INDEX, FIRST_TO_CHECK)                   \
    __attribute__((format (printf, (STRING_INDEX), (FIRST_TO_CHECK))))
#   define NON_NULL_PARAM(NUM) __attribute__((nonnull (NUM)));
#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
#   errro "Implement me for windows"
#else
#error "You will need constructor support for your compiler"
#endif
// #################################################



#ifdef _MSC_VER
#     define debug_break __debugbreak
#else
#    include <signal.h>

/* Use __builtin_trap() on AArch64 iOS only */

#    if defined(__i386__) || defined(__x86_64__)
__attribute__((gnu_inline, always_inline))
__inline__ static void trap_instruction(void)
{
    __asm__ volatile("int $0x03");
}
__attribute__((gnu_inline, always_inline))
__inline__ static void __debug_break(char* file, i32 line)
{
    fprintf(stderr, "Call to debug_break || file: %s || line: %d\n", file, line);
    fprintf(stderr, "Previous value of errno: %d\n", errno);
    fprintf(stderr, "    errno desc: %s", strerror(errno));
    fflush(stderr);
    raise(SIGTRAP);
}



#if defined __DEBUG
/* Note keep this in one liner to make the __LINE__ macro expansion work */
#define TEST_FUNCTION(test)                                             \
    static bool CONCAT(___first_time_called___, __LINE__) = false; if ( ((test) == true) && (CONCAT(___first_time_called___, __LINE__) == false) )  { CONCAT(___first_time_called___, __LINE__) = true; goto CONCAT(___exec___, __LINE__); } if (0)CONCAT(___exec___, __LINE__):
#else
#define TEST_FUNCTION(...)
#endif

#define debug_break() __debug_break( __FILE__, __LINE__)

#    else
#         error "Debug break needs to get written for this architecture"
#    endif
#endif





#if defined __DEBUG
#define LOG_STACK_TRACE() gdb_print_trace()
#else
#define LOG_STACK_TRACE() linux_print_stack_trace()
#endif


#ifdef __DEBUG
#    define assert_msg(X, ...)                  \
    do {                                        \
        if (!(X)) {                             \
            fprintf(stderr, __VA_ARGS__);       \
            fprintf(stderr, "\n");              \
            fflush(stderr);                     \
            LOG_STACK_TRACE();                  \
            debug_break();                      \
        }                                       \
    } while(0)

#    define assert(X) assert_msg(X, "Failed assertion at file: %s || line: %d\n", __FILE__, __LINE__)
#else
#    define assert_msg(X, ...)                  \
    do {                                        \
    } while(0)

#    define assert(X) do { } while(0)
#endif






// Usefull markers for in and output pointers inside a function
#define __OUT__
#define __IN__

#define ABS(X) ((X) > 0 ? (X) : -(X))


#define KILOBYTES(X) ((X) << 10)
#define MEGABYTES(X) (KILOBYTES(X) << 10)
#define PAGE_SIZE (KILOBYTES(4))


#define offset_of_member(STRUCT, MEMBER)        \
    (size_t) &(((STRUCT*)0)->MEMBER)


#define log_message(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#define dbglog(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#define invalid_code_path(...) assert_msg ( 0, "Invalid code path || file: %s || line: %d", __FILE__, __LINE__ )

#define memclr(SRC, SIZE)                       \
    memset((SRC), 0, (SIZE))

#define zero_struct(STRUCT)                     \
    memclr((STRUCT), sizeof(STRUCT))


char *
safe_strncpy(char *dest, const char *src, size_t n);

int
safe_snprintf(char *str, size_t size, const char *format, ...);

char *
safe_strncat(char *dest, const char *src, size_t n);

/* #define strncpy safe_strncpy */
/* #define strncat safe_strncat */

/* #define snprintf(str, size, format, ...) safe_snprintf(str, size, format, __VA_ARGS__) */

// #######################################################################
#endif /* UTILS_C_INCLUDE */
#if !defined UTILS_C_IMPLEMENTED && defined UTILS_C_IMPL
#define UTILS_C_IMPLEMENTED
// #######################################################################

#include <string.h>
#include <stdarg.h>


char *
safe_strncpy(char *dest, const char *src, size_t n)
{
    char * result = strncpy(dest, src, n);
    dest[n-1] = '\0';
    return result;
}

char *
safe_strncat(char *dest, const char *src, size_t n)
{
    char *result = strncat(dest, src, n);
    dest[n-1] = '\0';
    return result;
}




int
safe_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(str, size, format, ap);
    va_end(ap);
    str[size - 1] = 0;
    return result;
}


#endif /* UTILS_C_IMPL */
