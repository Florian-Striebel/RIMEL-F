












#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI int __popcountti2(ti_int a) {
tu_int x3 = (tu_int)a;
x3 = x3 - ((x3 >> 1) &
(((tu_int)0x5555555555555555uLL << 64) | 0x5555555555555555uLL));

x3 = ((x3 >> 2) &
(((tu_int)0x3333333333333333uLL << 64) | 0x3333333333333333uLL)) +
(x3 & (((tu_int)0x3333333333333333uLL << 64) | 0x3333333333333333uLL));

x3 = (x3 + (x3 >> 4)) &
(((tu_int)0x0F0F0F0F0F0F0F0FuLL << 64) | 0x0F0F0F0F0F0F0F0FuLL);

du_int x2 = (du_int)(x3 + (x3 >> 64));

su_int x = (su_int)(x2 + (x2 >> 32));

x = x + (x >> 16);



return (x + (x >> 8)) & 0xFF;
}

#endif
