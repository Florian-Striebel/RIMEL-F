













#define QUAD_PRECISION
#include "fp_lib.h"
#include "int_lib.h"










#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
COMPILER_RT_ABI fp_t __floatuntitf(tu_int a) {
if (a == 0)
return 0.0;
const unsigned N = sizeof(tu_int) * CHAR_BIT;
int sd = N - __clzti2(a);
int e = sd - 1;
if (sd > LDBL_MANT_DIG) {







switch (sd) {
case LDBL_MANT_DIG + 1:
a <<= 1;
break;
case LDBL_MANT_DIG + 2:
break;
default:
a = (a >> (sd - (LDBL_MANT_DIG + 2))) |
((a & ((tu_int)(-1) >> ((N + LDBL_MANT_DIG + 2) - sd))) != 0);
};

a |= (a & 4) != 0;
++a;
a >>= 2;

if (a & ((tu_int)1 << LDBL_MANT_DIG)) {
a >>= 1;
++e;
}

} else {
a <<= (LDBL_MANT_DIG - sd);

}

long_double_bits fb;
fb.u.high.all = (du_int)(e + 16383) << 48
| ((a >> 64) & 0x0000ffffffffffffLL);
fb.u.low.all = (du_int)(a);
return fb.f;
}

#endif
