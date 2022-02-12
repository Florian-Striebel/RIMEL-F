











#if !_ARCH_PPC

#include "int_lib.h"









COMPILER_RT_ABI long double __floatundixf(du_int a) {
if (a == 0)
return 0.0;
const unsigned N = sizeof(du_int) * CHAR_BIT;
int clz = __builtin_clzll(a);
int e = (N - 1) - clz;
long_double_bits fb;
fb.u.high.s.low = (e + 16383);
fb.u.low.all = a << clz;
return fb.f;
}

#endif
