











#include "int_lib.h"

typedef su_int fixuint_t;
typedef si_int fixint_t;
#include "int_div_impl.inc"



COMPILER_RT_ABI su_int __umodsi3(su_int a, su_int b) {
return __umodXi3(a, b);
}
