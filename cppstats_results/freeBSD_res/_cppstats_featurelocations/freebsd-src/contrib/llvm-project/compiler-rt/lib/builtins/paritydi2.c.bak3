











#include "int_lib.h"



COMPILER_RT_ABI int __paritydi2(di_int a) {
dwords x;
x.all = a;
su_int x2 = x.s.high ^ x.s.low;
x2 ^= x2 >> 16;
x2 ^= x2 >> 8;
x2 ^= x2 >> 4;
return (0x6996 >> (x2 & 0xF)) & 1;
}
