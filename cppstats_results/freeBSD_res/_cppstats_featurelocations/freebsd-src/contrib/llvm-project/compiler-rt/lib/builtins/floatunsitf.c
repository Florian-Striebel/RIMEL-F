













#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
COMPILER_RT_ABI fp_t __floatunsitf(unsigned int a) {

const int aWidth = sizeof a * CHAR_BIT;


if (a == 0)
return fromRep(0);


const int exponent = (aWidth - 1) - __builtin_clz(a);
rep_t result;


const int shift = significandBits - exponent;
result = (rep_t)a << shift ^ implicitBit;


result += (rep_t)(exponent + exponentBias) << significandBits;
return fromRep(result);
}

#endif
