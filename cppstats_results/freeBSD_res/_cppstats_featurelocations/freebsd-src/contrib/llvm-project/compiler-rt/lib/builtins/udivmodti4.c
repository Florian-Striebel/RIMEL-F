











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)






UNUSED
static inline du_int udiv128by64to64default(du_int u1, du_int u0, du_int v,
du_int *r) {
const unsigned n_udword_bits = sizeof(du_int) * CHAR_BIT;
const du_int b = (1ULL << (n_udword_bits / 2));
du_int un1, un0;
du_int vn1, vn0;
du_int q1, q0;
du_int un64, un21, un10;
du_int rhat;
si_int s;

s = __builtin_clzll(v);
if (s > 0) {

v = v << s;
un64 = (u1 << s) | (u0 >> (n_udword_bits - s));
un10 = u0 << s;
} else {

un64 = u1;
un10 = u0;
}


vn1 = v >> (n_udword_bits / 2);
vn0 = v & 0xFFFFFFFF;


un1 = un10 >> (n_udword_bits / 2);
un0 = un10 & 0xFFFFFFFF;


q1 = un64 / vn1;
rhat = un64 - q1 * vn1;


while (q1 >= b || q1 * vn0 > b * rhat + un1) {
q1 = q1 - 1;
rhat = rhat + vn1;
if (rhat >= b)
break;
}

un21 = un64 * b + un1 - q1 * v;


q0 = un21 / vn1;
rhat = un21 - q0 * vn1;


while (q0 >= b || q0 * vn0 > b * rhat + un0) {
q0 = q0 - 1;
rhat = rhat + vn1;
if (rhat >= b)
break;
}

*r = (un21 * b + un0 - q0 * v) >> s;
return q1 * b + q0;
}

static inline du_int udiv128by64to64(du_int u1, du_int u0, du_int v,
du_int *r) {
#if defined(__x86_64__)
du_int result;
__asm__("divq %[v]"
: "=a"(result), "=d"(*r)
: [ v ] "r"(v), "a"(u0), "d"(u1));
return result;
#else
return udiv128by64to64default(u1, u0, v, r);
#endif
}




COMPILER_RT_ABI tu_int __udivmodti4(tu_int a, tu_int b, tu_int *rem) {
const unsigned n_utword_bits = sizeof(tu_int) * CHAR_BIT;
utwords dividend;
dividend.all = a;
utwords divisor;
divisor.all = b;
utwords quotient;
utwords remainder;
if (divisor.all > dividend.all) {
if (rem)
*rem = dividend.all;
return 0;
}

if (divisor.s.high == 0) {
remainder.s.high = 0;
if (dividend.s.high < divisor.s.low) {

quotient.s.low = udiv128by64to64(dividend.s.high, dividend.s.low,
divisor.s.low, &remainder.s.low);
quotient.s.high = 0;
} else {


quotient.s.high = dividend.s.high / divisor.s.low;
dividend.s.high = dividend.s.high % divisor.s.low;
quotient.s.low = udiv128by64to64(dividend.s.high, dividend.s.low,
divisor.s.low, &remainder.s.low);
}
if (rem)
*rem = remainder.all;
return quotient.all;
}

si_int shift =
__builtin_clzll(divisor.s.high) - __builtin_clzll(dividend.s.high);
divisor.all <<= shift;
quotient.s.high = 0;
quotient.s.low = 0;
for (; shift >= 0; --shift) {
quotient.s.low <<= 1;






const ti_int s =
(ti_int)(divisor.all - dividend.all - 1) >> (n_utword_bits - 1);
quotient.s.low |= s & 1;
dividend.all -= divisor.all & s;
divisor.all >>= 1;
}
if (rem)
*rem = dividend.all;
return quotient.all;
}

#endif
