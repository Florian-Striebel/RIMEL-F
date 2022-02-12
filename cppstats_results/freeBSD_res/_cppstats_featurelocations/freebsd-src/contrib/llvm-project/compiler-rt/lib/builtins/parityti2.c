











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI int __parityti2(ti_int a) {
twords x;
dwords x2;
x.all = a;
x2.all = x.s.high ^ x.s.low;
su_int x3 = x2.s.high ^ x2.s.low;
x3 ^= x3 >> 16;
x3 ^= x3 >> 8;
x3 ^= x3 >> 4;
return (0x6996 >> (x3 & 0xF)) & 1;
}

#endif
