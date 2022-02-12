











#include "int_lib.h"



#define fixint_t di_int
#define fixuint_t du_int
#define COMPUTE_UDIV(a, b) __udivmoddi4((a), (b), (du_int *)0)
#include "int_div_impl.inc"

COMPILER_RT_ABI di_int __divdi3(di_int a, di_int b) { return __divXi3(a, b); }
