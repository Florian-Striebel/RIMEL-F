






























#include "gdtoaimp.h"

#if !defined(MULTIPLE_THREADS)
char *dtoa_result;
#endif

char *
#if defined(KR_headers)
rv_alloc(i) int i;
#else
rv_alloc(int i)
#endif
{
int j, k, *r;

j = sizeof(ULong);
for(k = 0;
sizeof(Bigint) - sizeof(ULong) - sizeof(int) + j <= i;
j <<= 1)
k++;
r = (int*)Balloc(k);
*r = k;
return
#if !defined(MULTIPLE_THREADS)
dtoa_result =
#endif
(char *)(r+1);
}

char *
#if defined(KR_headers)
nrv_alloc(s, rve, n) char *s, **rve; int n;
#else
nrv_alloc(char *s, char **rve, int n)
#endif
{
char *rv, *t;

t = rv = rv_alloc(n);
while((*t = *s++) !=0)
t++;
if (rve)
*rve = t;
return rv;
}







void
#if defined(KR_headers)
freedtoa(s) char *s;
#else
freedtoa(char *s)
#endif
{
Bigint *b = (Bigint *)((int *)s - 1);
b->maxwds = 1 << (b->k = *(int*)b);
Bfree(b);
#if !defined(MULTIPLE_THREADS)
if (s == dtoa_result)
dtoa_result = 0;
#endif
}

int
quorem
#if defined(KR_headers)
(b, S) Bigint *b, *S;
#else
(Bigint *b, Bigint *S)
#endif
{
int n;
ULong *bx, *bxe, q, *sx, *sxe;
#if defined(ULLong)
ULLong borrow, carry, y, ys;
#else
ULong borrow, carry, y, ys;
#if defined(Pack_32)
ULong si, z, zs;
#endif
#endif

n = S->wds;
#if defined(DEBUG)
if (b->wds > n)
Bug("oversize b in quorem");
#endif
if (b->wds < n)
return 0;
sx = S->x;
sxe = sx + --n;
bx = b->x;
bxe = bx + n;
q = *bxe / (*sxe + 1);
#if defined(DEBUG)
if (q > 9)
Bug("oversized quotient in quorem");
#endif
if (q) {
borrow = 0;
carry = 0;
do {
#if defined(ULLong)
ys = *sx++ * (ULLong)q + carry;
carry = ys >> 32;
y = *bx - (ys & 0xffffffffUL) - borrow;
borrow = y >> 32 & 1UL;
*bx++ = y & 0xffffffffUL;
#else
#if defined(Pack_32)
si = *sx++;
ys = (si & 0xffff) * q + carry;
zs = (si >> 16) * q + (ys >> 16);
carry = zs >> 16;
y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
z = (*bx >> 16) - (zs & 0xffff) - borrow;
borrow = (z & 0x10000) >> 16;
Storeinc(bx, z, y);
#else
ys = *sx++ * q + carry;
carry = ys >> 16;
y = *bx - (ys & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
*bx++ = y & 0xffff;
#endif
#endif
}
while(sx <= sxe);
if (!*bxe) {
bx = b->x;
while(--bxe > bx && !*bxe)
--n;
b->wds = n;
}
}
if (cmp(b, S) >= 0) {
q++;
borrow = 0;
carry = 0;
bx = b->x;
sx = S->x;
do {
#if defined(ULLong)
ys = *sx++ + carry;
carry = ys >> 32;
y = *bx - (ys & 0xffffffffUL) - borrow;
borrow = y >> 32 & 1UL;
*bx++ = y & 0xffffffffUL;
#else
#if defined(Pack_32)
si = *sx++;
ys = (si & 0xffff) + carry;
zs = (si >> 16) + (ys >> 16);
carry = zs >> 16;
y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
z = (*bx >> 16) - (zs & 0xffff) - borrow;
borrow = (z & 0x10000) >> 16;
Storeinc(bx, z, y);
#else
ys = *sx++ + carry;
carry = ys >> 16;
y = *bx - (ys & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
*bx++ = y & 0xffff;
#endif
#endif
}
while(sx <= sxe);
bx = b->x;
bxe = bx + n;
if (!*bxe) {
while(--bxe > bx && !*bxe)
--n;
b->wds = n;
}
}
return q;
}
