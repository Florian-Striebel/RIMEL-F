











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)












COMPILER_RT_ABI tu_int __fixunsxfti(long double a) {
long_double_bits fb;
fb.f = a;
int e = (fb.u.high.s.low & 0x00007FFF) - 16383;
if (e < 0 || (fb.u.high.s.low & 0x00008000))
return 0;
if ((unsigned)e > sizeof(tu_int) * CHAR_BIT)
return ~(tu_int)0;
tu_int r = fb.u.low.all;
if (e > 63)
r <<= (e - 63);
else
r >>= (63 - e);
return r;
}

#endif
