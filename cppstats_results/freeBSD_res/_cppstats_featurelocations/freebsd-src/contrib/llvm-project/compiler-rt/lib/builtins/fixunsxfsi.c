











#if !_ARCH_PPC

#include "int_lib.h"












#if defined(_MSC_VER) && !defined(__clang__)


#pragma warning(push)
#pragma warning(disable : 4700)
#endif

COMPILER_RT_ABI su_int __fixunsxfsi(long double a) {
long_double_bits fb;
fb.f = a;
int e = (fb.u.high.s.low & 0x00007FFF) - 16383;
if (e < 0 || (fb.u.high.s.low & 0x00008000))
return 0;
if ((unsigned)e > sizeof(su_int) * CHAR_BIT)
return ~(su_int)0;
return fb.u.low.s.high >> (31 - e);
}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif

#endif
