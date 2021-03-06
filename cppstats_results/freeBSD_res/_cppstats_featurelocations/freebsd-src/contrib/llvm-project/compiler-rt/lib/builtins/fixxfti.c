











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)











COMPILER_RT_ABI ti_int __fixxfti(long double a) {
const ti_int ti_max = (ti_int)((~(tu_int)0) / 2);
const ti_int ti_min = -ti_max - 1;
long_double_bits fb;
fb.f = a;
int e = (fb.u.high.s.low & 0x00007FFF) - 16383;
if (e < 0)
return 0;
ti_int s = -(si_int)((fb.u.high.s.low & 0x00008000) >> 15);
ti_int r = fb.u.low.all;
if ((unsigned)e >= sizeof(ti_int) * CHAR_BIT)
return a > 0 ? ti_max : ti_min;
if (e > 63)
r <<= (e - 63);
else
r >>= (63 - e);
return (r ^ s) - s;
}

#endif
