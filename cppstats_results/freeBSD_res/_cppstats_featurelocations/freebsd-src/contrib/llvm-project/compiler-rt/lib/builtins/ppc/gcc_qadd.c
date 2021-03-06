







#include "DD.h"

long double __gcc_qadd(long double x, long double y) {
static const uint32_t infinityHi = UINT32_C(0x7ff00000);

DD dst = {.ld = x}, src = {.ld = y};

register double A = dst.s.hi, a = dst.s.lo, B = src.s.hi, b = src.s.lo;


if ((A == 0.0) && (B == 0.0)) {
dst.s.hi = A + B;
dst.s.lo = 0.0;
return dst.ld;
}


const doublebits abits = {.d = A};
const doublebits bbits = {.d = B};
if ((((uint32_t)(abits.x >> 32) & infinityHi) == infinityHi) ||
(((uint32_t)(bbits.x >> 32) & infinityHi) == infinityHi)) {
dst.s.hi = A + B;
dst.s.lo = 0.0;
return dst.ld;
}




const double testForOverflow = A + (B + (a + b));
const doublebits testbits = {.d = testForOverflow};
if (((uint32_t)(testbits.x >> 32) & infinityHi) == infinityHi) {
dst.s.hi = testForOverflow;
dst.s.lo = 0.0;
return dst.ld;
}

double H, h;
double T, t;
double W, w;
double Y;

H = B + (A - (A + B));
T = b + (a - (a + b));
h = A + (B - (A + B));
t = a + (b - (a + b));

if (local_fabs(A) <= local_fabs(B))
w = (a + b) + h;
else
w = (a + b) + H;

W = (A + B) + w;
Y = (A + B) - W;
Y += w;

if (local_fabs(a) <= local_fabs(b))
w = t + Y;
else
w = T + Y;

dst.s.hi = Y = W + w;
dst.s.lo = (W - Y) + w;

return dst.ld;
}
