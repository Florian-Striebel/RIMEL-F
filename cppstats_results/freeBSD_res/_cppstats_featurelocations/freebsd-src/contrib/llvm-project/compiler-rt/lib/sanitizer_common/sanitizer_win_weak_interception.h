









#if !defined(SANITIZER_WIN_WEAK_INTERCEPTION_H)
#define SANITIZER_WIN_WEAK_INTERCEPTION_H
#include "sanitizer_internal_defs.h"

namespace __sanitizer {
int interceptWhenPossible(uptr dll_function, const char *real_function);
}




#define INTERCEPT_WEAK(Name) interceptWhenPossible((uptr) Name, #Name);

#define INTERCEPT_SANITIZER_WEAK_FUNCTION(Name) static int intercept_##Name() { return __sanitizer::interceptWhenPossible((__sanitizer::uptr) Name, #Name);} __pragma(section(".WEAK$M", long, read)) __declspec(allocate(".WEAK$M")) int (*__weak_intercept_##Name)() = intercept_##Name;







#endif
