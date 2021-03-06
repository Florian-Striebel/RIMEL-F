











#if !defined(ASAN_SUPPRESSIONS_H)
#define ASAN_SUPPRESSIONS_H

#include "asan_internal.h"
#include "sanitizer_common/sanitizer_stacktrace.h"

namespace __asan {

void InitializeSuppressions();
bool IsInterceptorSuppressed(const char *interceptor_name);
bool HaveStackTraceBasedSuppressions();
bool IsStackTraceSuppressed(const StackTrace *stack);
bool IsODRViolationSuppressed(const char *global_var_name);

}

#endif
