












#include "../int_math.h"


__int128_t __fixtfti(long double input) {


if (crt_isnan(input)) {
return ((__uint128_t)0x7FF8000000000000ll) << 64 |
(__uint128_t)0x0000000000000000ll;
}






if (input < 0.0) {
__uint128_t result = (__uint128_t)(-input);
return -((__int128_t)result);
}


__uint128_t result = (__uint128_t)input;
return result;
}
