













#define DOUBLE_PRECISION
#include "fp_lib.h"

#include "int_lib.h"

COMPILER_RT_ABI fp_t __floatunsidf(su_int a) {

const int aWidth = sizeof a * CHAR_BIT;


if (a == 0)
return fromRep(0);


const int exponent = (aWidth - 1) - clzsi(a);
rep_t result;


const int shift = significandBits - exponent;
result = (rep_t)a << shift ^ implicitBit;


result += (rep_t)(exponent + exponentBias) << significandBits;
return fromRep(result);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_ui2d(su_int a) { return __floatunsidf(a); }
#else
COMPILER_RT_ALIAS(__floatunsidf, __aeabi_ui2d)
#endif
#endif
