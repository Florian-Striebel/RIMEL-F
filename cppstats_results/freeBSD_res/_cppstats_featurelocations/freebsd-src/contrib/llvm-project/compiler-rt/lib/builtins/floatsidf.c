













#define DOUBLE_PRECISION
#include "fp_lib.h"

#include "int_lib.h"

COMPILER_RT_ABI fp_t __floatsidf(si_int a) {

const int aWidth = sizeof a * CHAR_BIT;


if (a == 0)
return fromRep(0);


rep_t sign = 0;
if (a < 0) {
sign = signBit;
a = -a;
}


const int exponent = (aWidth - 1) - clzsi(a);
rep_t result;




const int shift = significandBits - exponent;
result = (rep_t)(su_int)a << shift ^ implicitBit;


result += (rep_t)(exponent + exponentBias) << significandBits;

return fromRep(result | sign);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_i2d(si_int a) { return __floatsidf(a); }
#else
COMPILER_RT_ALIAS(__floatsidf, __aeabi_i2d)
#endif
#endif
