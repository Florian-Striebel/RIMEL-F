












#include "../int_math.h"
#define BIAS 1023


__uint128_t __fixunstfti(long double input) {


if (crt_isnan(input)) {
return ((__uint128_t)0x7FF8000000000000ll) << 64 |
(__uint128_t)0x0000000000000000ll;
}

__uint128_t result, hiResult, loResult;
int hiExponent, loExponent, shift;


union {
long double ld;
double d[2];
unsigned long long ull[2];
} ldUnion;



if (input < 1.0)
return 0;





ldUnion.ld = input;
hiExponent = ((ldUnion.ull[0] & 0x7FFFFFFFFFFFFFFFll) >> 52) - BIAS;
loExponent = ((ldUnion.ull[1] & 0x7FFFFFFFFFFFFFFFll) >> 52) - BIAS;



















if (hiExponent < 63) {
hiResult = (unsigned long long)ldUnion.d[0];
} else if (hiExponent < 128) {

shift = hiExponent - 54;
ldUnion.ull[0] &= 0x800FFFFFFFFFFFFFll;
ldUnion.ull[0] |= 0x4350000000000000ll;
hiResult = (unsigned long long)ldUnion.d[0];
hiResult <<= shift;
} else {




if (ldUnion.d[0] > 0) {
return ((__uint128_t)0xFFFFFFFFFFFFFFFFll) << 64 |
(__uint128_t)0xFFFFFFFFFFFFFFFFll;
} else {
return ((__uint128_t)0x0000000000000000ll) << 64 |
(__uint128_t)0x0000000000000001ll;
}
}


if (loExponent < 63) {
loResult = (long long)ldUnion.d[1];
} else {

shift = loExponent - 54;
ldUnion.ull[1] &= 0x800FFFFFFFFFFFFFll;
ldUnion.ull[1] |= 0x4350000000000000ll;
loResult = (long long)ldUnion.d[1];
loResult <<= shift;
}






ldUnion.ld = input;
if ((ldUnion.d[0] == (double)hiResult) &&
(ldUnion.d[1] < (double)((__int128_t)loResult)))
loResult--;


result = loResult + hiResult;
return result;
}
