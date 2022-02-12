











#include "int_lib.h"



#define fixint_t di_int
#define fixuint_t du_int
#define ASSIGN_UMOD(res, a, b) __udivmoddi4((a), (b), &(res))
#include "int_div_impl.inc"

COMPILER_RT_ABI di_int __moddi3(di_int a, di_int b) { return __modXi3(a, b); }
