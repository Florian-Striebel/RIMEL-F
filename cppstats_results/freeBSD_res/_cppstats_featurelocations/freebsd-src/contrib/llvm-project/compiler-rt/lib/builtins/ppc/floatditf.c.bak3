






#include "DD.h"

long double __floatditf(int64_t a) {

static const double twop32 = 0x1.0p32;
static const double twop52 = 0x1.0p52;

doublebits low = {.d = twop52};
low.x |= a & UINT64_C(0x00000000ffffffff);

const double high_addend = (double)((int32_t)(a >> 32)) * twop32 - twop52;








DD result;

result.s.hi = high_addend + low.d;
result.s.lo = (high_addend - result.s.hi) + low.d;

return result.ld;
}
