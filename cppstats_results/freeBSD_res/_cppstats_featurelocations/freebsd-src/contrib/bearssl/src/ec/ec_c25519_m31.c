























#include "inner.h"
























































#if BR_NO_ARITH_SHIFT
#define ARSH(x, n) (((uint32_t)(x) >> (n)) | ((-((uint32_t)(x) >> 31)) << (32 - (n))))

#else
#define ARSH(x, n) ((*(int32_t *)&(x)) >> (n))
#endif






static uint32_t
le8_to_le30(uint32_t *dst, const unsigned char *src, size_t len)
{
uint32_t acc;
int acc_len;

acc = 0;
acc_len = 0;
while (len -- > 0) {
uint32_t b;

b = *src ++;
if (acc_len < 22) {
acc |= b << acc_len;
acc_len += 8;
} else {
*dst ++ = (acc | (b << acc_len)) & 0x3FFFFFFF;
acc = b >> (30 - acc_len);
acc_len -= 22;
}
}
return acc;
}






static void
le30_to_le8(unsigned char *dst, size_t len, const uint32_t *src)
{
uint32_t acc;
int acc_len;

acc = 0;
acc_len = 0;
while (len -- > 0) {
if (acc_len < 8) {
uint32_t w;

w = *src ++;
*dst ++ = (unsigned char)(acc | (w << acc_len));
acc = w >> (8 - acc_len);
acc_len += 22;
} else {
*dst ++ = (unsigned char)acc;
acc >>= 8;
acc_len -= 8;
}
}
}






static void
mul9(uint32_t *d, const uint32_t *a, const uint32_t *b)
{











uint64_t t[17];
uint64_t cc;
int i;

t[ 0] = MUL31(a[0], b[0]);
t[ 1] = MUL31(a[0], b[1])
+ MUL31(a[1], b[0]);
t[ 2] = MUL31(a[0], b[2])
+ MUL31(a[1], b[1])
+ MUL31(a[2], b[0]);
t[ 3] = MUL31(a[0], b[3])
+ MUL31(a[1], b[2])
+ MUL31(a[2], b[1])
+ MUL31(a[3], b[0]);
t[ 4] = MUL31(a[0], b[4])
+ MUL31(a[1], b[3])
+ MUL31(a[2], b[2])
+ MUL31(a[3], b[1])
+ MUL31(a[4], b[0]);
t[ 5] = MUL31(a[0], b[5])
+ MUL31(a[1], b[4])
+ MUL31(a[2], b[3])
+ MUL31(a[3], b[2])
+ MUL31(a[4], b[1])
+ MUL31(a[5], b[0]);
t[ 6] = MUL31(a[0], b[6])
+ MUL31(a[1], b[5])
+ MUL31(a[2], b[4])
+ MUL31(a[3], b[3])
+ MUL31(a[4], b[2])
+ MUL31(a[5], b[1])
+ MUL31(a[6], b[0]);
t[ 7] = MUL31(a[0], b[7])
+ MUL31(a[1], b[6])
+ MUL31(a[2], b[5])
+ MUL31(a[3], b[4])
+ MUL31(a[4], b[3])
+ MUL31(a[5], b[2])
+ MUL31(a[6], b[1])
+ MUL31(a[7], b[0]);
t[ 8] = MUL31(a[0], b[8])
+ MUL31(a[1], b[7])
+ MUL31(a[2], b[6])
+ MUL31(a[3], b[5])
+ MUL31(a[4], b[4])
+ MUL31(a[5], b[3])
+ MUL31(a[6], b[2])
+ MUL31(a[7], b[1])
+ MUL31(a[8], b[0]);
t[ 9] = MUL31(a[1], b[8])
+ MUL31(a[2], b[7])
+ MUL31(a[3], b[6])
+ MUL31(a[4], b[5])
+ MUL31(a[5], b[4])
+ MUL31(a[6], b[3])
+ MUL31(a[7], b[2])
+ MUL31(a[8], b[1]);
t[10] = MUL31(a[2], b[8])
+ MUL31(a[3], b[7])
+ MUL31(a[4], b[6])
+ MUL31(a[5], b[5])
+ MUL31(a[6], b[4])
+ MUL31(a[7], b[3])
+ MUL31(a[8], b[2]);
t[11] = MUL31(a[3], b[8])
+ MUL31(a[4], b[7])
+ MUL31(a[5], b[6])
+ MUL31(a[6], b[5])
+ MUL31(a[7], b[4])
+ MUL31(a[8], b[3]);
t[12] = MUL31(a[4], b[8])
+ MUL31(a[5], b[7])
+ MUL31(a[6], b[6])
+ MUL31(a[7], b[5])
+ MUL31(a[8], b[4]);
t[13] = MUL31(a[5], b[8])
+ MUL31(a[6], b[7])
+ MUL31(a[7], b[6])
+ MUL31(a[8], b[5]);
t[14] = MUL31(a[6], b[8])
+ MUL31(a[7], b[7])
+ MUL31(a[8], b[6]);
t[15] = MUL31(a[7], b[8])
+ MUL31(a[8], b[7]);
t[16] = MUL31(a[8], b[8]);




cc = 0;
for (i = 0; i < 17; i ++) {
uint64_t w;

w = t[i] + cc;
d[i] = (uint32_t)w & 0x3FFFFFFF;
cc = w >> 30;
}
d[17] = (uint32_t)cc;
}





static void
square9(uint32_t *d, const uint32_t *a)
{
uint64_t t[17];
uint64_t cc;
int i;

t[ 0] = MUL31(a[0], a[0]);
t[ 1] = ((MUL31(a[0], a[1])) << 1);
t[ 2] = MUL31(a[1], a[1])
+ ((MUL31(a[0], a[2])) << 1);
t[ 3] = ((MUL31(a[0], a[3])
+ MUL31(a[1], a[2])) << 1);
t[ 4] = MUL31(a[2], a[2])
+ ((MUL31(a[0], a[4])
+ MUL31(a[1], a[3])) << 1);
t[ 5] = ((MUL31(a[0], a[5])
+ MUL31(a[1], a[4])
+ MUL31(a[2], a[3])) << 1);
t[ 6] = MUL31(a[3], a[3])
+ ((MUL31(a[0], a[6])
+ MUL31(a[1], a[5])
+ MUL31(a[2], a[4])) << 1);
t[ 7] = ((MUL31(a[0], a[7])
+ MUL31(a[1], a[6])
+ MUL31(a[2], a[5])
+ MUL31(a[3], a[4])) << 1);
t[ 8] = MUL31(a[4], a[4])
+ ((MUL31(a[0], a[8])
+ MUL31(a[1], a[7])
+ MUL31(a[2], a[6])
+ MUL31(a[3], a[5])) << 1);
t[ 9] = ((MUL31(a[1], a[8])
+ MUL31(a[2], a[7])
+ MUL31(a[3], a[6])
+ MUL31(a[4], a[5])) << 1);
t[10] = MUL31(a[5], a[5])
+ ((MUL31(a[2], a[8])
+ MUL31(a[3], a[7])
+ MUL31(a[4], a[6])) << 1);
t[11] = ((MUL31(a[3], a[8])
+ MUL31(a[4], a[7])
+ MUL31(a[5], a[6])) << 1);
t[12] = MUL31(a[6], a[6])
+ ((MUL31(a[4], a[8])
+ MUL31(a[5], a[7])) << 1);
t[13] = ((MUL31(a[5], a[8])
+ MUL31(a[6], a[7])) << 1);
t[14] = MUL31(a[7], a[7])
+ ((MUL31(a[6], a[8])) << 1);
t[15] = ((MUL31(a[7], a[8])) << 1);
t[16] = MUL31(a[8], a[8]);




cc = 0;
for (i = 0; i < 17; i ++) {
uint64_t w;

w = t[i] + cc;
d[i] = (uint32_t)w & 0x3FFFFFFF;
cc = w >> 30;
}
d[17] = (uint32_t)cc;
}








static uint32_t
reduce_final_f255(uint32_t *d)
{
uint32_t t[9];
uint32_t cc;
int i;

memcpy(t, d, sizeof t);
cc = 19;
for (i = 0; i < 9; i ++) {
uint32_t w;

w = t[i] + cc;
cc = w >> 30;
t[i] = w & 0x3FFFFFFF;
}
cc = t[8] >> 15;
t[8] &= 0x7FFF;
CCOPY(cc, d, t, sizeof t);
return cc;
}







static void
f255_mul(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
uint32_t t[18], cc;
int i;






mul9(t, a, b);











cc = MUL15(t[8] >> 15, 19);
t[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
uint64_t w;

w = (uint64_t)t[i] + (uint64_t)cc + MUL31(t[i + 9], 622592);
t[i] = (uint32_t)w & 0x3FFFFFFF;
cc = (uint32_t)(w >> 30);
}









cc = MUL15(t[8] >> 15, 19);
t[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
uint32_t z;

z = t[i] + cc;
d[i] = z & 0x3FFFFFFF;
cc = z >> 30;
}





}







static void
f255_square(uint32_t *d, const uint32_t *a)
{
uint32_t t[18], cc;
int i;






square9(t, a);





cc = MUL15(t[8] >> 15, 19);
t[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
uint64_t w;

w = (uint64_t)t[i] + (uint64_t)cc + MUL31(t[i + 9], 622592);
t[i] = (uint32_t)w & 0x3FFFFFFF;
cc = (uint32_t)(w >> 30);
}
cc = MUL15(t[8] >> 15, 19);
t[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
uint32_t z;

z = t[i] + cc;
d[i] = z & 0x3FFFFFFF;
cc = z >> 30;
}
}





static void
f255_add(uint32_t *d, const uint32_t *a, const uint32_t *b)
{




int i;
uint32_t cc, w;

cc = 0;
for (i = 0; i < 9; i ++) {
w = a[i] + b[i] + cc;
d[i] = w & 0x3FFFFFFF;
cc = w >> 30;
}
cc = MUL15(w >> 15, 19);
d[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
w = d[i] + cc;
d[i] = w & 0x3FFFFFFF;
cc = w >> 30;
}
}





static void
f255_sub(uint32_t *d, const uint32_t *a, const uint32_t *b)
{




int i;
uint32_t cc, w;

cc = (uint32_t)-38;
for (i = 0; i < 9; i ++) {
w = a[i] - b[i] + cc;
d[i] = w & 0x3FFFFFFF;
cc = ARSH(w, 30);
}
cc = MUL15((w + 0x10000) >> 15, 19);
d[8] &= 0x7FFF;
for (i = 0; i < 9; i ++) {
w = d[i] + cc;
d[i] = w & 0x3FFFFFFF;
cc = w >> 30;
}
}





static void
f255_mul_a24(uint32_t *d, const uint32_t *a)
{
int i;
uint64_t w;
uint32_t cc;







cc = 0;
for (i = 0; i < 8; i ++) {
w = MUL31(a[i], 121665) + (uint64_t)cc;
d[i] = (uint32_t)w & 0x3FFFFFFF;
cc = (uint32_t)(w >> 30);
}
w = MUL31(a[8], 121665) + (uint64_t)cc;
d[8] = (uint32_t)w & 0x7FFF;
cc = MUL15((uint32_t)(w >> 15), 19);

for (i = 0; i < 9; i ++) {
uint32_t z;

z = d[i] + cc;
d[i] = z & 0x3FFFFFFF;
cc = z >> 30;
}
}

static const unsigned char GEN[] = {
0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char ORDER[] = {
0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const unsigned char *
api_generator(int curve, size_t *len)
{
(void)curve;
*len = 32;
return GEN;
}

static const unsigned char *
api_order(int curve, size_t *len)
{
(void)curve;
*len = 32;
return ORDER;
}

static size_t
api_xoff(int curve, size_t *len)
{
(void)curve;
*len = 32;
return 0;
}

static void
cswap(uint32_t *a, uint32_t *b, uint32_t ctl)
{
int i;

ctl = -ctl;
for (i = 0; i < 9; i ++) {
uint32_t aw, bw, tw;

aw = a[i];
bw = b[i];
tw = ctl & (aw ^ bw);
a[i] = aw ^ tw;
b[i] = bw ^ tw;
}
}

static uint32_t
api_mul(unsigned char *G, size_t Glen,
const unsigned char *kb, size_t kblen, int curve)
{
uint32_t x1[9], x2[9], x3[9], z2[9], z3[9];
uint32_t a[9], aa[9], b[9], bb[9];
uint32_t c[9], d[9], e[9], da[9], cb[9];
unsigned char k[32];
uint32_t swap;
int i;

(void)curve;







if (Glen != 32 || kblen > 32) {
return 0;
}
G[31] &= 0x7F;





x1[8] = le8_to_le30(x1, G, 32);
memcpy(x3, x1, sizeof x1);
memset(z2, 0, sizeof z2);
memset(x2, 0, sizeof x2);
x2[0] = 1;
memset(z3, 0, sizeof z3);
z3[0] = 1;

memset(k, 0, (sizeof k) - kblen);
memcpy(k + (sizeof k) - kblen, kb, kblen);
k[31] &= 0xF8;
k[0] &= 0x7F;
k[0] |= 0x40;





swap = 0;
for (i = 254; i >= 0; i --) {
uint32_t kt;

kt = (k[31 - (i >> 3)] >> (i & 7)) & 1;
swap ^= kt;
cswap(x2, x3, swap);
cswap(z2, z3, swap);
swap = kt;








f255_add(a, x2, z2);
f255_square(aa, a);
f255_sub(b, x2, z2);
f255_square(bb, b);
f255_sub(e, aa, bb);
f255_add(c, x3, z3);
f255_sub(d, x3, z3);
f255_mul(da, d, a);
f255_mul(cb, c, b);













f255_add(x3, da, cb);
f255_square(x3, x3);
f255_sub(z3, da, cb);
f255_square(z3, z3);
f255_mul(z3, z3, x1);
f255_mul(x2, aa, bb);
f255_mul_a24(z2, e);
f255_add(z2, z2, aa);
f255_mul(z2, e, z2);







}
cswap(x2, x3, swap);
cswap(z2, z3, swap);






memcpy(a, z2, sizeof z2);
for (i = 0; i < 15; i ++) {
f255_square(a, a);
f255_mul(a, a, z2);
}
memcpy(b, a, sizeof a);
for (i = 0; i < 14; i ++) {
int j;

for (j = 0; j < 16; j ++) {
f255_square(b, b);
}
f255_mul(b, b, a);
}
for (i = 14; i >= 0; i --) {
f255_square(b, b);
if ((0xFFEB >> i) & 1) {
f255_mul(b, z2, b);
}
}
f255_mul(x2, x2, b);
reduce_final_f255(x2);
le30_to_le8(G, 32, x2);
return 1;
}

static size_t
api_mulgen(unsigned char *R,
const unsigned char *x, size_t xlen, int curve)
{
const unsigned char *G;
size_t Glen;

G = api_generator(curve, &Glen);
memcpy(R, G, Glen);
api_mul(R, Glen, x, xlen, curve);
return Glen;
}

static uint32_t
api_muladd(unsigned char *A, const unsigned char *B, size_t len,
const unsigned char *x, size_t xlen,
const unsigned char *y, size_t ylen, int curve)
{





(void)A;
(void)B;
(void)len;
(void)x;
(void)xlen;
(void)y;
(void)ylen;
(void)curve;
return 0;
}


const br_ec_impl br_ec_c25519_m31 = {
(uint32_t)0x20000000,
&api_generator,
&api_order,
&api_xoff,
&api_mul,
&api_mulgen,
&api_muladd
};
