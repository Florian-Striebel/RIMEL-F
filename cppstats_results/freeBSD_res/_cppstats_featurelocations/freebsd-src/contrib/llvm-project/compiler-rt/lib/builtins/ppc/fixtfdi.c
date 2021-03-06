






#include "../int_math.h"
#include "DD.h"

uint64_t __fixtfdi(long double input) {
const DD x = {.ld = input};
const doublebits hibits = {.d = x.s.hi};

const uint32_t absHighWord =
(uint32_t)(hibits.x >> 32) & UINT32_C(0x7fffffff);
const uint32_t absHighWordMinusOne = absHighWord - UINT32_C(0x3ff00000);


if (UINT32_C(0x03f00000) > absHighWordMinusOne) {

const int unbiasedHeadExponent = absHighWordMinusOne >> 20;

int64_t result = hibits.x & INT64_C(0x000fffffffffffff);
result |= INT64_C(0x0010000000000000);
result <<= 10;

const int64_t hiNegationMask = ((int64_t)(hibits.x)) >> 63;


if (0.0 != x.s.lo) {
const doublebits lobits = {.d = x.s.lo};
int64_t tailMantissa = lobits.x & INT64_C(0x000fffffffffffff);
tailMantissa |= INT64_C(0x0010000000000000);



const int64_t loNegationMask = ((int64_t)(lobits.x)) >> 63;
const int64_t negationMask = loNegationMask ^ hiNegationMask;
tailMantissa = (tailMantissa ^ negationMask) - negationMask;



const int biasedTailExponent = (int)(lobits.x >> 52) & 0x7ff;



tailMantissa >>=
(unbiasedHeadExponent - (biasedTailExponent - (1023 - 10)));

result += tailMantissa;
}

result >>= (62 - unbiasedHeadExponent);


result = (result ^ hiNegationMask) - hiNegationMask;
return result;
}




if (1.0 > crt_fabs(x.s.hi))
return INT64_C(0);


if (x.s.hi == -0x1.0p63) {

int64_t result = INT64_MIN;

if (0.0 < x.s.lo) {



const doublebits lobits = {.d = x.s.lo};
int64_t tailMantissa = lobits.x & INT64_C(0x000fffffffffffff);
tailMantissa |= INT64_C(0x0010000000000000);


tailMantissa = (tailMantissa ^ INT64_C(-1)) + INT64_C(1);


const int biasedTailExponent = (int)(lobits.x >> 52) & 0x7ff;
tailMantissa >>= 1075 - biasedTailExponent;

result -= tailMantissa;
}

return result;
}


if (x.s.hi > 0.0)
return INT64_MAX;
else
return INT64_MIN;
}
