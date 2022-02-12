











#include "int_lib.h"



#define fixint_t si_int
#define fixuint_t su_int




#define COMPUTE_UDIV(a, b) ((su_int)(a) / (su_int)(b))
#include "int_div_impl.inc"

COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b) { return __divXi3(a, b); }

#if defined(__ARM_EABI__)
COMPILER_RT_ALIAS(__divsi3, __aeabi_idiv)
#endif
