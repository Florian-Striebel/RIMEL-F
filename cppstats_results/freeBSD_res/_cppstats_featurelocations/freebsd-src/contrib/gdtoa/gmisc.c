






























#include "gdtoaimp.h"

void
#if defined(KR_headers)
rshift(b, k) Bigint *b; int k;
#else
rshift(Bigint *b, int k)
#endif
{
ULong *x, *x1, *xe, y;
int n;

x = x1 = b->x;
n = k >> kshift;
if (n < b->wds) {
xe = x + b->wds;
x += n;
if (k &= kmask) {
n = ULbits - k;
y = *x++ >> k;
while(x < xe) {
*x1++ = (y | (*x << n)) & ALL_ON;
y = *x++ >> k;
}
if ((*x1 = y) !=0)
x1++;
}
else
while(x < xe)
*x1++ = *x++;
}
if ((b->wds = x1 - b->x) == 0)
b->x[0] = 0;
}

int
#if defined(KR_headers)
trailz(b) Bigint *b;
#else
trailz(Bigint *b)
#endif
{
ULong L, *x, *xe;
int n = 0;

x = b->x;
xe = x + b->wds;
for(n = 0; x < xe && !*x; x++)
n += ULbits;
if (x < xe) {
L = *x;
n += lo0bits(&L);
}
return n;
}
