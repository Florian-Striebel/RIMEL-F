







#include "int_lib.h"










#if defined(KERNEL_USE)

NORETURN extern void panic(const char *, ...);
#if !defined(_WIN32)
__attribute__((visibility("hidden")))
#endif
void __compilerrt_abort_impl(const char *file, int line, const char *function) {
panic("%s:%d: abort in %s", file, line, function);
}

#elif __APPLE__


NORETURN extern void __assert_rtn(const char *func, const char *file, int line,
const char *message);

__attribute__((weak))
__attribute__((visibility("hidden")))
void __compilerrt_abort_impl(const char *file, int line, const char *function) {
__assert_rtn(function, file, line, "libcompiler_rt abort");
}

#else

#if defined(_WIN32)
#include <stdlib.h>
#endif

#if !defined(_WIN32)
__attribute__((weak))
__attribute__((visibility("hidden")))
#endif
void __compilerrt_abort_impl(const char *file, int line, const char *function) {
#if !__STDC_HOSTED__

__builtin_trap();
#elif defined(_WIN32)
abort();
#else
__builtin_abort();
#endif
}

#endif
