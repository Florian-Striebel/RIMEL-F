











#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)



COMPILER_RT_ABI long double __powitf2(long double a, int b) {
const int recip = b < 0;
long double r = 1;
while (1) {
if (b & 1)
r *= a;
b /= 2;
if (b == 0)
break;
a *= a;
}
return recip ? 1 / r : r;
}

#endif
