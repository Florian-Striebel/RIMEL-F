











#include "int_lib.h"

typedef su_int fixuint_t;
typedef si_int fixint_t;
#include "int_div_impl.inc"



COMPILER_RT_ABI su_int __udivsi3(su_int a, su_int b) {
return __udivXi3(a, b);
}

#if defined(__ARM_EABI__)
COMPILER_RT_ALIAS(__udivsi3, __aeabi_uidiv)
#endif
