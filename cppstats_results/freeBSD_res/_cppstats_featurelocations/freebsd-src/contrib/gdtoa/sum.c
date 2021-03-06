






























#include "gdtoaimp.h"

Bigint *
#if defined(KR_headers)
sum(a, b) Bigint *a; Bigint *b;
#else
sum(Bigint *a, Bigint *b)
#endif
{
Bigint *c;
ULong carry, *xc, *xa, *xb, *xe, y;
#if defined(Pack_32)
ULong z;
#endif

if (a->wds < b->wds) {
c = b; b = a; a = c;
}
c = Balloc(a->k);
c->wds = a->wds;
carry = 0;
xa = a->x;
xb = b->x;
xc = c->x;
xe = xc + b->wds;
#if defined(Pack_32)
do {
y = (*xa & 0xffff) + (*xb & 0xffff) + carry;
carry = (y & 0x10000) >> 16;
z = (*xa++ >> 16) + (*xb++ >> 16) + carry;
carry = (z & 0x10000) >> 16;
Storeinc(xc, z, y);
}
while(xc < xe);
xe += a->wds - b->wds;
while(xc < xe) {
y = (*xa & 0xffff) + carry;
carry = (y & 0x10000) >> 16;
z = (*xa++ >> 16) + carry;
carry = (z & 0x10000) >> 16;
Storeinc(xc, z, y);
}
#else
do {
y = *xa++ + *xb++ + carry;
carry = (y & 0x10000) >> 16;
*xc++ = y & 0xffff;
}
while(xc < xe);
xe += a->wds - b->wds;
while(xc < xe) {
y = *xa++ + carry;
carry = (y & 0x10000) >> 16;
*xc++ = y & 0xffff;
}
#endif
if (carry) {
if (c->wds == c->maxwds) {
b = Balloc(c->k + 1);
Bcopy(b, c);
Bfree(c);
c = b;
}
c->x[c->wds++] = 1;
}
return c;
}
