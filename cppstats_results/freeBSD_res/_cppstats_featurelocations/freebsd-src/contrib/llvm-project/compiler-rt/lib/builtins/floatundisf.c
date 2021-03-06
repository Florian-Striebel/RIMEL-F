


















#include "int_lib.h"

COMPILER_RT_ABI float __floatundisf(du_int a) {
if (a == 0)
return 0.0F;
const unsigned N = sizeof(du_int) * CHAR_BIT;
int sd = N - __builtin_clzll(a);
si_int e = sd - 1;
if (sd > FLT_MANT_DIG) {







switch (sd) {
case FLT_MANT_DIG + 1:
a <<= 1;
break;
case FLT_MANT_DIG + 2:
break;
default:
a = (a >> (sd - (FLT_MANT_DIG + 2))) |
((a & ((du_int)(-1) >> ((N + FLT_MANT_DIG + 2) - sd))) != 0);
};

a |= (a & 4) != 0;
++a;
a >>= 2;

if (a & ((du_int)1 << FLT_MANT_DIG)) {
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

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI float __aeabi_ul2f(du_int a) { return __floatundisf(a); }
#else
COMPILER_RT_ALIAS(__floatundisf, __aeabi_ul2f)
#endif
#endif
