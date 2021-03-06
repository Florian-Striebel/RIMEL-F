













#define SINGLE_PRECISION
#include "fp_lib.h"

#include "int_lib.h"

COMPILER_RT_ABI fp_t __floatsisf(int a) {

const int aWidth = sizeof a * CHAR_BIT;


if (a == 0)
return fromRep(0);


rep_t sign = 0;
if (a < 0) {
sign = signBit;
a = -a;
}


const int exponent = (aWidth - 1) - __builtin_clz(a);
rep_t result;


if (exponent <= significandBits) {
const int shift = significandBits - exponent;
result = (rep_t)a << shift ^ implicitBit;
} else {
const int shift = exponent - significandBits;
result = (rep_t)a >> shift ^ implicitBit;
rep_t round = (rep_t)a << (typeWidth - shift);
if (round > signBit)
result++;
if (round == signBit)
result += result & 1;
}


result += (rep_t)(exponent + exponentBias) << significandBits;

return fromRep(result | sign);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_i2f(int a) { return __floatsisf(a); }
#else
COMPILER_RT_ALIAS(__floatsisf, __aeabi_i2f)
#endif
#endif
