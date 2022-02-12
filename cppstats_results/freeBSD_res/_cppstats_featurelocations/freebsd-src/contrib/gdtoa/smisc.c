






























#include "gdtoaimp.h"

Bigint *
s2b
#if defined(KR_headers)
(s, nd0, nd, y9, dplen) CONST char *s; int dplen, nd0, nd; ULong y9;
#else
(CONST char *s, int nd0, int nd, ULong y9, int dplen)
#endif
{
Bigint *b;
int i, k;
Long x, y;

x = (nd + 8) / 9;
for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#if defined(Pack_32)
b = Balloc(k);
b->x[0] = y9;
b->wds = 1;
#else
b = Balloc(k+1);
b->x[0] = y9 & 0xffff;
b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

i = 9;
if (9 < nd0) {
s += 9;
do b = multadd(b, 10, *s++ - '0');
while(++i < nd0);
s += dplen;
}
else
s += dplen + 9;
for(; i < nd; i++)
b = multadd(b, 10, *s++ - '0');
return b;
}

double
ratio
#if defined(KR_headers)
(a, b) Bigint *a, *b;
#else
(Bigint *a, Bigint *b)
#endif
{
U da, db;
int k, ka, kb;

dval(&da) = b2d(a, &ka);
dval(&db) = b2d(b, &kb);
k = ka - kb + ULbits*(a->wds - b->wds);
#if defined(IBM)
if (k > 0) {
word0(&da) += (k >> 2)*Exp_msk1;
if (k &= 3)
dval(&da) *= 1 << k;
}
else {
k = -k;
word0(&db) += (k >> 2)*Exp_msk1;
if (k &= 3)
dval(&db) *= 1 << k;
}
#else
if (k > 0)
word0(&da) += k*Exp_msk1;
else {
k = -k;
word0(&db) += k*Exp_msk1;
}
#endif
return dval(&da) / dval(&db);
}

#if defined(INFNAN_CHECK)

int
match
#if defined(KR_headers)
(sp, t) char **sp, *t;
#else
(CONST char **sp, char *t)
#endif
{
int c, d;
CONST char *s = *sp;

while( (d = *t++) !=0) {
if ((c = *++s) >= 'A' && c <= 'Z')
c += 'a' - 'A';
if (c != d)
return 0;
}
*sp = s + 1;
return 1;
}
#endif

void
#if defined(KR_headers)
copybits(c, n, b) ULong *c; int n; Bigint *b;
#else
copybits(ULong *c, int n, Bigint *b)
#endif
{
ULong *ce, *x, *xe;
#if defined(Pack_16)
int nw, nw1;
#endif

ce = c + ((n-1) >> kshift) + 1;
x = b->x;
#if defined(Pack_32)
xe = x + b->wds;
while(x < xe)
*c++ = *x++;
#else
nw = b->wds;
nw1 = nw & 1;
for(xe = x + (nw - nw1); x < xe; x += 2)
Storeinc(c, x[1], x[0]);
if (nw1)
*c++ = *x;
#endif
while(c < ce)
*c++ = 0;
}

ULong
#if defined(KR_headers)
any_on(b, k) Bigint *b; int k;
#else
any_on(Bigint *b, int k)
#endif
{
int n, nwds;
ULong *x, *x0, x1, x2;

x = b->x;
nwds = b->wds;
n = k >> kshift;
if (n > nwds)
n = nwds;
else if (n < nwds && (k &= kmask)) {
x1 = x2 = x[n];
x1 >>= k;
x1 <<= k;
if (x1 != x2)
return 1;
}
x0 = x;
x += n;
while(x > x0)
if (*--x)
return 1;
return 0;
}
