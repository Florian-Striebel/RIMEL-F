











#include "int_lib.h"





COMPILER_RT_ABI int __clzsi2(si_int a) {
su_int x = (su_int)a;
si_int t = ((x & 0xFFFF0000) == 0) << 4;
x >>= 16 - t;
su_int r = t;

t = ((x & 0xFF00) == 0) << 3;
x >>= 8 - t;
r += t;

t = ((x & 0xF0) == 0) << 2;
x >>= 4 - t;
r += t;

t = ((x & 0xC) == 0) << 1;
x >>= 2 - t;
r += t;











return r + ((2 - x) & -((x & 2) == 0));
}
