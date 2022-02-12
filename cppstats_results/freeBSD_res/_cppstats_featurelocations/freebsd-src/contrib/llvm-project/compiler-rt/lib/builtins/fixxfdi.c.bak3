











#if !_ARCH_PPC

#include "int_lib.h"











#if defined(_MSC_VER) && !defined(__clang__)


#pragma warning(push)
#pragma warning(disable : 4700)
#endif

COMPILER_RT_ABI di_int __fixxfdi(long double a) {
const di_int di_max = (di_int)((~(du_int)0) / 2);
const di_int di_min = -di_max - 1;
long_double_bits fb;
fb.f = a;
int e = (fb.u.high.s.low & 0x00007FFF) - 16383;
if (e < 0)
return 0;
if ((unsigned)e >= sizeof(di_int) * CHAR_BIT)
return a > 0 ? di_max : di_min;
di_int s = -(si_int)((fb.u.high.s.low & 0x00008000) >> 15);
di_int r = fb.u.low.all;
r = (du_int)r >> (63 - e);
return (r ^ s) - s;
}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif

#endif
