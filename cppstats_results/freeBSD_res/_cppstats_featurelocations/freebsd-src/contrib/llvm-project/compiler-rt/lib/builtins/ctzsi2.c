











#include "int_lib.h"





COMPILER_RT_ABI int __ctzsi2(si_int a) {
su_int x = (su_int)a;
si_int t = ((x & 0x0000FFFF) == 0)
<< 4;
x >>= t;
su_int r = t;

t = ((x & 0x00FF) == 0) << 3;
x >>= t;
r += t;

t = ((x & 0x0F) == 0) << 2;
x >>= t;
r += t;

t = ((x & 0x3) == 0) << 1;
x >>= t;
x &= 3;
r += t;














return r + ((2 - (x >> 1)) & -((x & 1) == 0));
}
