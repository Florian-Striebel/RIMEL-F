























#include "inner.h"

#if BR_INT128 || BR_UMUL128

#if BR_UMUL128
#include <intrin.h>
#endif

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







#define MASK51 (((uint64_t)1 << 51) - (uint64_t)1)




static inline void
f255_cswap(uint64_t *a, uint64_t *b, uint32_t ctl)
{
uint64_t m, w;

m = -(uint64_t)ctl;
w = m & (a[0] ^ b[0]); a[0] ^= w; b[0] ^= w;
w = m & (a[1] ^ b[1]); a[1] ^= w; b[1] ^= w;
w = m & (a[2] ^ b[2]); a[2] ^= w; b[2] ^= w;
w = m & (a[3] ^ b[3]); a[3] ^= w; b[3] ^= w;
w = m & (a[4] ^ b[4]); a[4] ^= w; b[4] ^= w;
}




static inline void
f255_add(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
d[0] = a[0] + b[0];
d[1] = a[1] + b[1];
d[2] = a[2] + b[2];
d[3] = a[3] + b[3];
d[4] = a[4] + b[4];
}








static inline void
f255_sub(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
uint64_t cc, w;












w = a[0] - b[0] - 19456;
d[0] = w & MASK51;
cc = -(w >> 51) & 0x3FF;
w = a[1] - b[1] - cc;
d[1] = w & MASK51;
cc = -(w >> 51) & 0x3FF;
w = a[2] - b[2] - cc;
d[2] = w & MASK51;
cc = -(w >> 51) & 0x3FF;
w = a[3] - b[3] - cc;
d[3] = w & MASK51;
cc = -(w >> 51) & 0x3FF;
d[4] = ((uint64_t)1 << 61) + a[4] - b[4] - cc;






d[0] += 19 * (d[4] >> 51);
d[4] &= MASK51;
}










#if BR_INT128

#define UMUL51(hi, lo, x, y) do { unsigned __int128 umul_tmp; umul_tmp = (unsigned __int128)(x) * (unsigned __int128)(y); (hi) = (uint64_t)(umul_tmp >> 51); (lo) = (uint64_t)umul_tmp & MASK51; } while (0)






#elif BR_UMUL128

#define UMUL51(hi, lo, x, y) do { uint64_t umul_hi, umul_lo; umul_lo = _umul128((x), (y), &umul_hi); (hi) = (umul_hi << 13) | (umul_lo >> 51); (lo) = umul_lo & MASK51; } while (0)






#endif







static inline void
f255_mul(uint64_t *d, uint64_t *a, uint64_t *b)
{
uint64_t t[10], hi, lo, w, cc;











UMUL51(t[1], t[0], a[0], b[0]);

UMUL51(t[2], lo, a[1], b[0]); t[1] += lo;
UMUL51(hi, lo, a[0], b[1]); t[1] += lo; t[2] += hi;

UMUL51(t[3], lo, a[2], b[0]); t[2] += lo;
UMUL51(hi, lo, a[1], b[1]); t[2] += lo; t[3] += hi;
UMUL51(hi, lo, a[0], b[2]); t[2] += lo; t[3] += hi;

UMUL51(t[4], lo, a[3], b[0]); t[3] += lo;
UMUL51(hi, lo, a[2], b[1]); t[3] += lo; t[4] += hi;
UMUL51(hi, lo, a[1], b[2]); t[3] += lo; t[4] += hi;
UMUL51(hi, lo, a[0], b[3]); t[3] += lo; t[4] += hi;

UMUL51(t[5], lo, a[4], b[0]); t[4] += lo;
UMUL51(hi, lo, a[3], b[1]); t[4] += lo; t[5] += hi;
UMUL51(hi, lo, a[2], b[2]); t[4] += lo; t[5] += hi;
UMUL51(hi, lo, a[1], b[3]); t[4] += lo; t[5] += hi;
UMUL51(hi, lo, a[0], b[4]); t[4] += lo; t[5] += hi;

UMUL51(t[6], lo, a[4], b[1]); t[5] += lo;
UMUL51(hi, lo, a[3], b[2]); t[5] += lo; t[6] += hi;
UMUL51(hi, lo, a[2], b[3]); t[5] += lo; t[6] += hi;
UMUL51(hi, lo, a[1], b[4]); t[5] += lo; t[6] += hi;

UMUL51(t[7], lo, a[4], b[2]); t[6] += lo;
UMUL51(hi, lo, a[3], b[3]); t[6] += lo; t[7] += hi;
UMUL51(hi, lo, a[2], b[4]); t[6] += lo; t[7] += hi;

UMUL51(t[8], lo, a[4], b[3]); t[7] += lo;
UMUL51(hi, lo, a[3], b[4]); t[7] += lo; t[8] += hi;

UMUL51(t[9], lo, a[4], b[4]); t[8] += lo;










t[0] += 19 * t[5];
t[1] += 19 * t[6];
t[2] += 19 * t[7];
t[3] += 19 * t[8];
t[4] += 19 * t[9];




w = t[0];
d[0] = w & MASK51;
cc = w >> 51;
w = t[1] + cc;
d[1] = w & MASK51;
cc = w >> 51;
w = t[2] + cc;
d[2] = w & MASK51;
cc = w >> 51;
w = t[3] + cc;
d[3] = w & MASK51;
cc = w >> 51;
w = t[4] + cc;
d[4] = w & MASK51;
cc = w >> 51;






d[0] += 19 * cc;
}





static inline void
f255_mul_a24(uint64_t *d, const uint64_t *a)
{
uint64_t t[5], cc, w;





w = a[0] * 15;
t[0] = w & MASK51;
cc = w >> 51;
w = a[1] * 15 + cc;
t[1] = w & MASK51;
cc = w >> 51;
w = a[2] * 15 + cc;
t[2] = w & MASK51;
cc = w >> 51;
w = a[3] * 15 + cc;
t[3] = w & MASK51;
cc = w >> 51;
w = a[4] * 15 + cc;
t[4] = w & MASK51;
t[0] += 19 * (w >> 51);






w = t[0] * 8111;
d[0] = w & MASK51;
cc = w >> 51;
w = t[1] * 8111 + cc;
d[1] = w & MASK51;
cc = w >> 51;
w = t[2] * 8111 + cc;
d[2] = w & MASK51;
cc = w >> 51;
w = t[3] * 8111 + cc;
d[3] = w & MASK51;
cc = w >> 51;
w = t[4] * 8111 + cc;
d[4] = w & MASK51;
d[0] += 19 * (w >> 51);
}






static inline void
f255_final_reduce(uint64_t *a)
{
uint64_t t[5], cc, w;







w = a[0] + 19;
t[0] = w & MASK51;
cc = w >> 51;
w = a[1] + cc;
t[1] = w & MASK51;
cc = w >> 51;
w = a[2] + cc;
t[2] = w & MASK51;
cc = w >> 51;
w = a[3] + cc;
t[3] = w & MASK51;
cc = w >> 51;
w = a[4] + cc;
t[4] = w & MASK51;
cc = w >> 51;





cc = -cc;
a[0] ^= cc & (a[0] ^ t[0]);
a[1] ^= cc & (a[1] ^ t[1]);
a[2] ^= cc & (a[2] ^ t[2]);
a[3] ^= cc & (a[3] ^ t[3]);
a[4] ^= cc & (a[4] ^ t[4]);
}

static uint32_t
api_mul(unsigned char *G, size_t Glen,
const unsigned char *kb, size_t kblen, int curve)
{
unsigned char k[32];
uint64_t x1[5], x2[5], z2[5], x3[5], z3[5];
uint32_t swap;
int i;

(void)curve;





if (Glen != 32 || kblen > 32) {
return 0;
}






x1[0] = br_dec64le(&G[0]) & MASK51;
x1[1] = (br_dec64le(&G[6]) >> 3) & MASK51;
x1[2] = (br_dec64le(&G[12]) >> 6) & MASK51;
x1[3] = (br_dec64le(&G[19]) >> 1) & MASK51;
x1[4] = (br_dec64le(&G[24]) >> 12) & MASK51;






memset(x2, 0, sizeof x2);
x2[0] = 1;
memset(z2, 0, sizeof z2);
memcpy(x3, x1, sizeof x1);
memcpy(z3, x2, sizeof x2);





memset(k, 0, (sizeof k) - kblen);
memcpy(k + (sizeof k) - kblen, kb, kblen);
k[31] &= 0xF8;
k[0] &= 0x7F;
k[0] |= 0x40;

swap = 0;

for (i = 254; i >= 0; i --) {
uint64_t a[5], aa[5], b[5], bb[5], e[5];
uint64_t c[5], d[5], da[5], cb[5];
uint32_t kt;

kt = (k[31 - (i >> 3)] >> (i & 7)) & 1;
swap ^= kt;
f255_cswap(x2, x3, swap);
f255_cswap(z2, z3, swap);
swap = kt;













f255_add(a, x2, z2);


f255_mul(aa, a, a);


f255_sub(b, x2, z2);


f255_mul(bb, b, b);


f255_sub(e, aa, bb);


f255_add(c, x3, z3);


f255_sub(d, x3, z3);


f255_mul(da, d, a);


f255_mul(cb, c, b);


f255_add(x3, da, cb);
f255_mul(x3, x3, x3);


f255_sub(z3, da, cb);
f255_mul(z3, z3, z3);
f255_mul(z3, x1, z3);


f255_mul(x2, aa, bb);


f255_mul_a24(z2, e);
f255_add(z2, aa, z2);
f255_mul(z2, e, z2);
}

f255_cswap(x2, x3, swap);
f255_cswap(z2, z3, swap);





memcpy(x1, z2, sizeof z2);
for (i = 0; i < 15; i ++) {
f255_mul(x1, x1, x1);
f255_mul(x1, x1, z2);
}
memcpy(x3, x1, sizeof x1);
for (i = 0; i < 14; i ++) {
int j;

for (j = 0; j < 16; j ++) {
f255_mul(x3, x3, x3);
}
f255_mul(x3, x3, x1);
}
for (i = 14; i >= 0; i --) {
f255_mul(x3, x3, x3);
if ((0xFFEB >> i) & 1) {
f255_mul(x3, z2, x3);
}
}




f255_mul(x2, x2, x3);
f255_final_reduce(x2);





x2[0] |= x2[1] << 51;
x2[1] = (x2[1] >> 13) | (x2[2] << 38);
x2[2] = (x2[2] >> 26) | (x2[3] << 25);
x2[3] = (x2[3] >> 39) | (x2[4] << 12);
br_enc64le(G, x2[0]);
br_enc64le(G + 8, x2[1]);
br_enc64le(G + 16, x2[2]);
br_enc64le(G + 24, x2[3]);
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


const br_ec_impl br_ec_c25519_m62 = {
(uint32_t)0x20000000,
&api_generator,
&api_order,
&api_xoff,
&api_mul,
&api_mulgen,
&api_muladd
};


const br_ec_impl *
br_ec_c25519_m62_get(void)
{
return &br_ec_c25519_m62;
}

#else


const br_ec_impl *
br_ec_c25519_m62_get(void)
{
return 0;
}

#endif
