











#if !_ARCH_PPC

#include "int_lib.h"










COMPILER_RT_ABI long double __floatdixf(di_int a) {
if (a == 0)
return 0.0;
const unsigned N = sizeof(di_int) * CHAR_BIT;
const di_int s = a >> (N - 1);
a = (a ^ s) - s;
int clz = __builtin_clzll(a);
int e = (N - 1) - clz;
long_double_bits fb;
fb.u.high.s.low = ((su_int)s & 0x00008000) |
(e + 16383);
fb.u.low.all = a << clz;
return fb.f;
}

#endif
