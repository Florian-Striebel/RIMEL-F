#if !defined(JEMALLOC_INTERNAL_UTIL_H)
#define JEMALLOC_INTERNAL_UTIL_H

#define UTIL_INLINE static inline


#if !defined(JEMALLOC_ALLOC_JUNK)
#define JEMALLOC_ALLOC_JUNK ((uint8_t)0xa5)
#endif
#if !defined(JEMALLOC_FREE_JUNK)
#define JEMALLOC_FREE_JUNK ((uint8_t)0x5a)
#endif





#define JEMALLOC_ARG_CONCAT(...) __VA_ARGS__


#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)






#define JEMALLOC_CC_SILENCE_INIT(v) = v

#if defined(__GNUC__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) !!(x)
#define unlikely(x) !!(x)
#endif

#if !defined(JEMALLOC_INTERNAL_UNREACHABLE)
#error JEMALLOC_INTERNAL_UNREACHABLE should have been defined by configure
#endif

#define unreachable() JEMALLOC_INTERNAL_UNREACHABLE()


UTIL_INLINE void
set_errno(int errnum) {
#if defined(_WIN32)
SetLastError(errnum);
#else
errno = errnum;
#endif
}


UTIL_INLINE int
get_errno(void) {
#if defined(_WIN32)
return GetLastError();
#else
return errno;
#endif
}

#undef UTIL_INLINE

#endif
