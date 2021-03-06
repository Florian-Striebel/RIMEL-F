






























#include "gdtoaimp.h"

static Bigint *freelist[Kmax+1];
#if !defined(Omit_Private_Memory)
#if !defined(PRIVATE_MEM)
#define PRIVATE_MEM 2304
#endif
#define PRIVATE_mem ((PRIVATE_MEM+sizeof(double)-1)/sizeof(double))
static double private_mem[PRIVATE_mem], *pmem_next = private_mem;
#endif

Bigint *
Balloc
#if defined(KR_headers)
(k) int k;
#else
(int k)
#endif
{
int x;
Bigint *rv;
#if !defined(Omit_Private_Memory)
unsigned int len;
#endif

ACQUIRE_DTOA_LOCK(0);


if (k <= Kmax && (rv = freelist[k]) !=0) {
freelist[k] = rv->next;
}
else {
x = 1 << k;
#if defined(Omit_Private_Memory)
rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(ULong));
#else
len = (sizeof(Bigint) + (x-1)*sizeof(ULong) + sizeof(double) - 1)
/sizeof(double);
if (k <= Kmax && pmem_next - private_mem + len <= PRIVATE_mem) {
rv = (Bigint*)pmem_next;
pmem_next += len;
}
else
rv = (Bigint*)MALLOC(len*sizeof(double));
#endif
rv->k = k;
rv->maxwds = x;
}
FREE_DTOA_LOCK(0);
rv->sign = rv->wds = 0;
return rv;
}

void
Bfree
#if defined(KR_headers)
(v) Bigint *v;
#else
(Bigint *v)
#endif
{
if (v) {
if (v->k > Kmax)
#if defined(FREE)
FREE((void*)v);
#else
free((void*)v);
#endif
else {
ACQUIRE_DTOA_LOCK(0);
v->next = freelist[v->k];
freelist[v->k] = v;
FREE_DTOA_LOCK(0);
}
}
}

int
lo0bits
#if defined(KR_headers)
(y) ULong *y;
#else
(ULong *y)
#endif
{
int k;
ULong x = *y;

if (x & 7) {
if (x & 1)
return 0;
if (x & 2) {
*y = x >> 1;
return 1;
}
*y = x >> 2;
return 2;
}
k = 0;
if (!(x & 0xffff)) {
k = 16;
x >>= 16;
}
if (!(x & 0xff)) {
k += 8;
x >>= 8;
}
if (!(x & 0xf)) {
k += 4;
x >>= 4;
}
if (!(x & 0x3)) {
k += 2;
x >>= 2;
}
if (!(x & 1)) {
k++;
x >>= 1;
if (!x)
return 32;
}
*y = x;
return k;
}

Bigint *
multadd
#if defined(KR_headers)
(b, m, a) Bigint *b; int m, a;
#else
(Bigint *b, int m, int a)
#endif
{
int i, wds;
#if defined(ULLong)
ULong *x;
ULLong carry, y;
#else
ULong carry, *x, y;
#if defined(Pack_32)
ULong xi, z;
#endif
#endif
Bigint *b1;

wds = b->wds;
x = b->x;
i = 0;
carry = a;
do {
#if defined(ULLong)
y = *x * (ULLong)m + carry;
carry = y >> 32;
*x++ = y & 0xffffffffUL;
#else
#if defined(Pack_32)
xi = *x;
y = (xi & 0xffff) * m + carry;
z = (xi >> 16) * m + (y >> 16);
carry = z >> 16;
*x++ = (z << 16) + (y & 0xffff);
#else
y = *x * m + carry;
carry = y >> 16;
*x++ = y & 0xffff;
#endif
#endif
}
while(++i < wds);
if (carry) {
if (wds >= b->maxwds) {
b1 = Balloc(b->k+1);
Bcopy(b1, b);
Bfree(b);
b = b1;
}
b->x[wds++] = carry;
b->wds = wds;
}
return b;
}

int
hi0bits_D2A
#if defined(KR_headers)
(x) ULong x;
#else
(ULong x)
#endif
{
int k = 0;

if (!(x & 0xffff0000)) {
k = 16;
x <<= 16;
}
if (!(x & 0xff000000)) {
k += 8;
x <<= 8;
}
if (!(x & 0xf0000000)) {
k += 4;
x <<= 4;
}
if (!(x & 0xc0000000)) {
k += 2;
x <<= 2;
}
if (!(x & 0x80000000)) {
k++;
if (!(x & 0x40000000))
return 32;
}
return k;
}

Bigint *
i2b
#if defined(KR_headers)
(i) int i;
#else
(int i)
#endif
{
Bigint *b;

b = Balloc(1);
b->x[0] = i;
b->wds = 1;
return b;
}

Bigint *
mult
#if defined(KR_headers)
(a, b) Bigint *a, *b;
#else
(Bigint *a, Bigint *b)
#endif
{
Bigint *c;
int k, wa, wb, wc;
ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
ULong y;
#if defined(ULLong)
ULLong carry, z;
#else
ULong carry, z;
#if defined(Pack_32)
ULong z2;
#endif
#endif

if (a->wds < b->wds) {
c = a;
a = b;
b = c;
}
k = a->k;
wa = a->wds;
wb = b->wds;
wc = wa + wb;
if (wc > a->maxwds)
k++;
c = Balloc(k);
for(x = c->x, xa = x + wc; x < xa; x++)
*x = 0;
xa = a->x;
xae = xa + wa;
xb = b->x;
xbe = xb + wb;
xc0 = c->x;
#if defined(ULLong)
for(; xb < xbe; xc0++) {
if ( (y = *xb++) !=0) {
x = xa;
xc = xc0;
carry = 0;
do {
z = *x++ * (ULLong)y + *xc + carry;
carry = z >> 32;
*xc++ = z & 0xffffffffUL;
}
while(x < xae);
*xc = carry;
}
}
#else
#if defined(Pack_32)
for(; xb < xbe; xb++, xc0++) {
if ( (y = *xb & 0xffff) !=0) {
x = xa;
xc = xc0;
carry = 0;
do {
z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
carry = z >> 16;
z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
carry = z2 >> 16;
Storeinc(xc, z2, z);
}
while(x < xae);
*xc = carry;
}
if ( (y = *xb >> 16) !=0) {
x = xa;
xc = xc0;
carry = 0;
z2 = *xc;
do {
z = (*x & 0xffff) * y + (*xc >> 16) + carry;
carry = z >> 16;
Storeinc(xc, z, z2);
z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
carry = z2 >> 16;
}
while(x < xae);
*xc = z2;
}
}
#else
for(; xb < xbe; xc0++) {
if ( (y = *xb++) !=0) {
x = xa;
xc = xc0;
carry = 0;
do {
z = *x++ * y + *xc + carry;
carry = z >> 16;
*xc++ = z & 0xffff;
}
while(x < xae);
*xc = carry;
}
}
#endif
#endif
for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
c->wds = wc;
return c;
}

static Bigint *p5s;

Bigint *
pow5mult
#if defined(KR_headers)
(b, k) Bigint *b; int k;
#else
(Bigint *b, int k)
#endif
{
Bigint *b1, *p5, *p51;
int i;
static int p05[3] = { 5, 25, 125 };

if ( (i = k & 3) !=0)
b = multadd(b, p05[i-1], 0);

if (!(k >>= 2))
return b;
if ((p5 = p5s) == 0) {

#if defined(MULTIPLE_THREADS)
ACQUIRE_DTOA_LOCK(1);
if (!(p5 = p5s)) {
p5 = p5s = i2b(625);
p5->next = 0;
}
FREE_DTOA_LOCK(1);
#else
p5 = p5s = i2b(625);
p5->next = 0;
#endif
}
for(;;) {
if (k & 1) {
b1 = mult(b, p5);
Bfree(b);
b = b1;
}
if (!(k >>= 1))
break;
if ((p51 = p5->next) == 0) {
#if defined(MULTIPLE_THREADS)
ACQUIRE_DTOA_LOCK(1);
if (!(p51 = p5->next)) {
p51 = p5->next = mult(p5,p5);
p51->next = 0;
}
FREE_DTOA_LOCK(1);
#else
p51 = p5->next = mult(p5,p5);
p51->next = 0;
#endif
}
p5 = p51;
}
return b;
}

Bigint *
lshift
#if defined(KR_headers)
(b, k) Bigint *b; int k;
#else
(Bigint *b, int k)
#endif
{
int i, k1, n, n1;
Bigint *b1;
ULong *x, *x1, *xe, z;

n = k >> kshift;
k1 = b->k;
n1 = n + b->wds + 1;
for(i = b->maxwds; n1 > i; i <<= 1)
k1++;
b1 = Balloc(k1);
x1 = b1->x;
for(i = 0; i < n; i++)
*x1++ = 0;
x = b->x;
xe = x + b->wds;
if (k &= kmask) {
#if defined(Pack_32)
k1 = 32 - k;
z = 0;
do {
*x1++ = *x << k | z;
z = *x++ >> k1;
}
while(x < xe);
if ((*x1 = z) !=0)
++n1;
#else
k1 = 16 - k;
z = 0;
do {
*x1++ = *x << k & 0xffff | z;
z = *x++ >> k1;
}
while(x < xe);
if (*x1 = z)
++n1;
#endif
}
else do
*x1++ = *x++;
while(x < xe);
b1->wds = n1 - 1;
Bfree(b);
return b1;
}

int
cmp
#if defined(KR_headers)
(a, b) Bigint *a, *b;
#else
(Bigint *a, Bigint *b)
#endif
{
ULong *xa, *xa0, *xb, *xb0;
int i, j;

i = a->wds;
j = b->wds;
#if defined(DEBUG)
if (i > 1 && !a->x[i-1])
Bug("cmp called with a->x[a->wds-1] == 0");
if (j > 1 && !b->x[j-1])
Bug("cmp called with b->x[b->wds-1] == 0");
#endif
if (i -= j)
return i;
xa0 = a->x;
xa = xa0 + j;
xb0 = b->x;
xb = xb0 + j;
for(;;) {
if (*--xa != *--xb)
return *xa < *xb ? -1 : 1;
if (xa <= xa0)
break;
}
return 0;
}

Bigint *
diff
#if defined(KR_headers)
(a, b) Bigint *a, *b;
#else
(Bigint *a, Bigint *b)
#endif
{
Bigint *c;
int i, wa, wb;
ULong *xa, *xae, *xb, *xbe, *xc;
#if defined(ULLong)
ULLong borrow, y;
#else
ULong borrow, y;
#if defined(Pack_32)
ULong z;
#endif
#endif

i = cmp(a,b);
if (!i) {
c = Balloc(0);
c->wds = 1;
c->x[0] = 0;
return c;
}
if (i < 0) {
c = a;
a = b;
b = c;
i = 1;
}
else
i = 0;
c = Balloc(a->k);
c->sign = i;
wa = a->wds;
xa = a->x;
xae = xa + wa;
wb = b->wds;
xb = b->x;
xbe = xb + wb;
xc = c->x;
borrow = 0;
#if defined(ULLong)
do {
y = (ULLong)*xa++ - *xb++ - borrow;
borrow = y >> 32 & 1UL;
*xc++ = y & 0xffffffffUL;
}
while(xb < xbe);
while(xa < xae) {
y = *xa++ - borrow;
borrow = y >> 32 & 1UL;
*xc++ = y & 0xffffffffUL;
}
#else
#if defined(Pack_32)
do {
y = (*xa & 0xffff) - (*xb & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
z = (*xa++ >> 16) - (*xb++ >> 16) - borrow;
borrow = (z & 0x10000) >> 16;
Storeinc(xc, z, y);
}
while(xb < xbe);
while(xa < xae) {
y = (*xa & 0xffff) - borrow;
borrow = (y & 0x10000) >> 16;
z = (*xa++ >> 16) - borrow;
borrow = (z & 0x10000) >> 16;
Storeinc(xc, z, y);
}
#else
do {
y = *xa++ - *xb++ - borrow;
borrow = (y & 0x10000) >> 16;
*xc++ = y & 0xffff;
}
while(xb < xbe);
while(xa < xae) {
y = *xa++ - borrow;
borrow = (y & 0x10000) >> 16;
*xc++ = y & 0xffff;
}
#endif
#endif
while(!*--xc)
wa--;
c->wds = wa;
return c;
}

double
b2d
#if defined(KR_headers)
(a, e) Bigint *a; int *e;
#else
(Bigint *a, int *e)
#endif
{
ULong *xa, *xa0, w, y, z;
int k;
U d;
#if defined(VAX)
ULong d0, d1;
#else
#define d0 word0(&d)
#define d1 word1(&d)
#endif

xa0 = a->x;
xa = xa0 + a->wds;
y = *--xa;
#if defined(DEBUG)
if (!y) Bug("zero y in b2d");
#endif
k = hi0bits(y);
*e = 32 - k;
#if defined(Pack_32)
if (k < Ebits) {
d0 = Exp_1 | y >> (Ebits - k);
w = xa > xa0 ? *--xa : 0;
d1 = y << ((32-Ebits) + k) | w >> (Ebits - k);
goto ret_d;
}
z = xa > xa0 ? *--xa : 0;
if (k -= Ebits) {
d0 = Exp_1 | y << k | z >> (32 - k);
y = xa > xa0 ? *--xa : 0;
d1 = z << k | y >> (32 - k);
}
else {
d0 = Exp_1 | y;
d1 = z;
}
#else
if (k < Ebits + 16) {
z = xa > xa0 ? *--xa : 0;
d0 = Exp_1 | y << k - Ebits | z >> Ebits + 16 - k;
w = xa > xa0 ? *--xa : 0;
y = xa > xa0 ? *--xa : 0;
d1 = z << k + 16 - Ebits | w << k - Ebits | y >> 16 + Ebits - k;
goto ret_d;
}
z = xa > xa0 ? *--xa : 0;
w = xa > xa0 ? *--xa : 0;
k -= Ebits + 16;
d0 = Exp_1 | y << k + 16 | z << k | w >> 16 - k;
y = xa > xa0 ? *--xa : 0;
d1 = w << k + 16 | y << k;
#endif
ret_d:
#if defined(VAX)
word0(&d) = d0 >> 16 | d0 << 16;
word1(&d) = d1 >> 16 | d1 << 16;
#endif
return dval(&d);
}
#undef d0
#undef d1

Bigint *
d2b
#if defined(KR_headers)
(dd, e, bits) double dd; int *e, *bits;
#else
(double dd, int *e, int *bits)
#endif
{
Bigint *b;
U d;
#if !defined(Sudden_Underflow)
int i;
#endif
int de, k;
ULong *x, y, z;
#if defined(VAX)
ULong d0, d1;
#else
#define d0 word0(&d)
#define d1 word1(&d)
#endif
d.d = dd;
#if defined(VAX)
d0 = word0(&d) >> 16 | word0(&d) << 16;
d1 = word1(&d) >> 16 | word1(&d) << 16;
#endif

#if defined(Pack_32)
b = Balloc(1);
#else
b = Balloc(2);
#endif
x = b->x;

z = d0 & Frac_mask;
d0 &= 0x7fffffff;
#if defined(Sudden_Underflow)
de = (int)(d0 >> Exp_shift);
#if !defined(IBM)
z |= Exp_msk11;
#endif
#else
if ( (de = (int)(d0 >> Exp_shift)) !=0)
z |= Exp_msk1;
#endif
#if defined(Pack_32)
if ( (y = d1) !=0) {
if ( (k = lo0bits(&y)) !=0) {
x[0] = y | z << (32 - k);
z >>= k;
}
else
x[0] = y;
#if !defined(Sudden_Underflow)
i =
#endif
b->wds = (x[1] = z) !=0 ? 2 : 1;
}
else {
k = lo0bits(&z);
x[0] = z;
#if !defined(Sudden_Underflow)
i =
#endif
b->wds = 1;
k += 32;
}
#else
if ( (y = d1) !=0) {
if ( (k = lo0bits(&y)) !=0)
if (k >= 16) {
x[0] = y | z << 32 - k & 0xffff;
x[1] = z >> k - 16 & 0xffff;
x[2] = z >> k;
i = 2;
}
else {
x[0] = y & 0xffff;
x[1] = y >> 16 | z << 16 - k & 0xffff;
x[2] = z >> k & 0xffff;
x[3] = z >> k+16;
i = 3;
}
else {
x[0] = y & 0xffff;
x[1] = y >> 16;
x[2] = z & 0xffff;
x[3] = z >> 16;
i = 3;
}
}
else {
#if defined(DEBUG)
if (!z)
Bug("Zero passed to d2b");
#endif
k = lo0bits(&z);
if (k >= 16) {
x[0] = z;
i = 0;
}
else {
x[0] = z & 0xffff;
x[1] = z >> 16;
i = 1;
}
k += 32;
}
while(!x[i])
--i;
b->wds = i + 1;
#endif
#if !defined(Sudden_Underflow)
if (de) {
#endif
#if defined(IBM)
*e = (de - Bias - (P-1) << 2) + k;
*bits = 4*P + 8 - k - hi0bits(word0(&d) & Frac_mask);
#else
*e = de - Bias - (P-1) + k;
*bits = P - k;
#endif
#if !defined(Sudden_Underflow)
}
else {
*e = de - Bias - (P-1) + 1 + k;
#if defined(Pack_32)
*bits = 32*i - hi0bits(x[i-1]);
#else
*bits = (i+2)*16 - hi0bits(x[i]);
#endif
}
#endif
return b;
}
#undef d0
#undef d1

CONST double
#if defined(IEEE_Arith)
bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
CONST double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128, 1e-256
};
#else
#if defined(IBM)
bigtens[] = { 1e16, 1e32, 1e64 };
CONST double tinytens[] = { 1e-16, 1e-32, 1e-64 };
#else
bigtens[] = { 1e16, 1e32 };
CONST double tinytens[] = { 1e-16, 1e-32 };
#endif
#endif

CONST double
tens[] = {
1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
1e20, 1e21, 1e22
#if defined(VAX)
, 1e23, 1e24
#endif
};

char *
#if defined(KR_headers)
strcp_D2A(a, b) char *a; char *b;
#else
strcp_D2A(char *a, CONST char *b)
#endif
{
while((*a = *b++))
a++;
return a;
}

#if defined(NO_STRING_H)

Char *
#if defined(KR_headers)
memcpy_D2A(a, b, len) Char *a; Char *b; size_t len;
#else
memcpy_D2A(void *a1, void *b1, size_t len)
#endif
{
char *a = (char*)a1, *ae = a + len;
char *b = (char*)b1, *a0 = a;
while(a < ae)
*a++ = *b++;
return a0;
}

#endif
