







#include "int_lib.h"
#include <stdio.h>







#if !defined(_WIN32)
__attribute__((visibility("hidden")))
#endif
COMPILER_RT_ABI void
__eprintf(const char *format, const char *assertion_expression,
const char *line, const char *file) {
fprintf(stderr, format, assertion_expression, line, file);
fflush(stderr);
compilerrt_abort();
}
