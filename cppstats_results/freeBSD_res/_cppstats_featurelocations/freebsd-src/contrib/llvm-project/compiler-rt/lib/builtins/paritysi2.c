











#include "int_lib.h"



COMPILER_RT_ABI int __paritysi2(si_int a) {
su_int x = (su_int)a;
x ^= x >> 16;
x ^= x >> 8;
x ^= x >> 4;
return (0x6996 >> (x & 0xF)) & 1;
}
