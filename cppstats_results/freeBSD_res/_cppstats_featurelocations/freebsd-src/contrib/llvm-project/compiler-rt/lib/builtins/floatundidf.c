



















#include "int_lib.h"

#if !defined(__SOFTFP__)



COMPILER_RT_ABI double __floatundidf(du_int a) {
static const double twop52 = 4503599627370496.0;
static const double twop84 = 19342813113834066795298816.0;
static const double twop84_plus_twop52 =
19342813118337666422669312.0;

union {
uint64_t x;
double d;
} high = {.d = twop84};
union {
uint64_t x;
double d;
} low = {.d = twop52};

high.x |= a >> 32;
low.x |= a & UINT64_C(0x00000000ffffffff);

const double result = (high.d - twop84_plus_twop52) + low.d;
return result;
}

#else




COMPILER_RT_ABI double __floatundidf(du_int a) {
if (a == 0)
return 0.0;
const unsigned N = sizeof(du_int) * CHAR_BIT;
int sd = N - __builtin_clzll(a);
int e = sd - 1;
if (sd > DBL_MANT_DIG) {







switch (sd) {
case DBL_MANT_DIG + 1:
a <<= 1;
break;
case DBL_MANT_DIG + 2:
break;
default:
a = (a >> (sd - (DBL_MANT_DIG + 2))) |
((a & ((du_int)(-1) >> ((N + DBL_MANT_DIG + 2) - sd))) != 0);
};

a |= (a & 4) != 0;
++a;
a >>= 2;

if (a & ((du_int)1 << DBL_MANT_DIG)) {
a >>= 1;
++e;
}

} else {
a <<= (DBL_MANT_DIG - sd);

}
double_bits fb;
fb.u.s.high = ((su_int)(e + 1023) << 20) |
((su_int)(a >> 32) & 0x000FFFFF);
fb.u.s.low = (su_int)a;
return fb.f;
}
#endif

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI double __aeabi_ul2d(du_int a) { return __floatundidf(a); }
#else
COMPILER_RT_ALIAS(__floatundidf, __aeabi_ul2d)
#endif
#endif
