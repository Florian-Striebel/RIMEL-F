











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI int __clzti2(ti_int a) {
twords x;
x.all = a;
const di_int f = -(x.s.high == 0);
return __builtin_clzll((x.s.high & ~f) | (x.s.low & f)) +
((si_int)f & ((si_int)(sizeof(di_int) * CHAR_BIT)));
}

#endif
