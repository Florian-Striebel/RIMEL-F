











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



#define fixint_t ti_int
#define fixuint_t tu_int
#define COMPUTE_UDIV(a, b) __udivmodti4((a), (b), (tu_int *)0)
#include "int_div_impl.inc"

COMPILER_RT_ABI ti_int __divti3(ti_int a, ti_int b) { return __divXi3(a, b); }

#endif
