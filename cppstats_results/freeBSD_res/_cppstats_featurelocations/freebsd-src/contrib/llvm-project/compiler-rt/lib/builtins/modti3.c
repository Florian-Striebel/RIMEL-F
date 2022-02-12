











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



#define fixint_t ti_int
#define fixuint_t tu_int
#define ASSIGN_UMOD(res, a, b) __udivmodti4((a), (b), &(res))
#include "int_div_impl.inc"

COMPILER_RT_ABI ti_int __modti3(ti_int a, ti_int b) { return __modXi3(a, b); }

#endif
