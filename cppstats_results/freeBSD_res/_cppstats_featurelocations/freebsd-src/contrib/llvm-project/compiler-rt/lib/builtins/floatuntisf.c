











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)








COMPILER_RT_ABI float __floatuntisf(tu_int a) {
if (a == 0)
return 0.0F;
const unsigned N = sizeof(tu_int) * CHAR_BIT;
int sd = N - __clzti2(a);
int e = sd - 1;
if (sd > FLT_MANT_DIG) {







switch (sd) {
case FLT_MANT_DIG + 1:
a <<= 1;
break;
case FLT_MANT_DIG + 2:
break;
default:
a = (a >> (sd - (FLT_MANT_DIG + 2))) |
((a & ((tu_int)(-1) >> ((N + FLT_MANT_DIG + 2) - sd))) != 0);
};

a |= (a & 4) != 0;
++a;
a >>= 2;

if (a & ((tu_int)1 << FLT_MANT_DIG)) {
a >>= 1;
++e;
}

} else {
a <<= (FLT_MANT_DIG - sd);

}
float_bits fb;
fb.u = ((e + 127) << 23) |
((su_int)a & 0x007FFFFF);
return fb.f;
}

#endif
