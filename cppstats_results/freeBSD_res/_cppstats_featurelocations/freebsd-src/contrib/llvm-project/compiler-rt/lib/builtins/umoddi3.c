











#include "int_lib.h"

typedef du_int fixuint_t;
typedef di_int fixint_t;
#include "int_div_impl.inc"



COMPILER_RT_ABI du_int __umoddi3(du_int a, du_int b) {
return __umodXi3(a, b);
}
