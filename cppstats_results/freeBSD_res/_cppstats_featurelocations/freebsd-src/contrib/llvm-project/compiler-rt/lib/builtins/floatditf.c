













#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
COMPILER_RT_ABI fp_t __floatditf(di_int a) {

const int aWidth = sizeof a * CHAR_BIT;


if (a == 0)
return fromRep(0);


rep_t sign = 0;
du_int aAbs = (du_int)a;
if (a < 0) {
sign = signBit;
aAbs = ~(du_int)a + 1U;
}


const int exponent = (aWidth - 1) - __builtin_clzll(aAbs);
rep_t result;


const int shift = significandBits - exponent;
result = (rep_t)aAbs << shift ^ implicitBit;


result += (rep_t)(exponent + exponentBias) << significandBits;

return fromRep(result | sign);
}

#endif
