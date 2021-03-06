






#include "DD.h"

long double __floatunditf(uint64_t a) {



static const double twop52 = 0x1.0p52;
static const double twop84 = 0x1.0p84;
static const double twop84_plus_twop52 = 0x1.00000001p84;

doublebits high = {.d = twop84};
doublebits low = {.d = twop52};

high.x |= a >> 32;
low.x |= a & UINT64_C(0x00000000ffffffff);

const double high_addend = high.d - twop84_plus_twop52;








DD result;

result.s.hi = high_addend + low.d;
result.s.lo = (high_addend - result.s.hi) + low.d;

return result.ld;
}
