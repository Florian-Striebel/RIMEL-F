























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







#define MASK63 (((uint64_t)1 << 63) - (uint64_t)1)




static inline void
f255_cswap(uint64_t *a, uint64_t *b, uint32_t ctl)
{
uint64_t m, w;

m = -(uint64_t)ctl;
w = m & (a[0] ^ b[0]); a[0] ^= w; b[0] ^= w;
w = m & (a[1] ^ b[1]); a[1] ^= w; b[1] ^= w;
w = m & (a[2] ^ b[2]); a[2] ^= w; b[2] ^= w;
w = m & (a[3] ^ b[3]); a[3] ^= w; b[3] ^= w;
}




static inline void
f255_add(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
#if BR_INT128

uint64_t t0, t1, t2, t3, cc;
unsigned __int128 z;

z = (unsigned __int128)a[0] + (unsigned __int128)b[0];
t0 = (uint64_t)z;
z = (unsigned __int128)a[1] + (unsigned __int128)b[1] + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)a[2] + (unsigned __int128)b[2] + (z >> 64);
t2 = (uint64_t)z;
z = (unsigned __int128)a[3] + (unsigned __int128)b[3] + (z >> 64);
t3 = (uint64_t)z & MASK63;
cc = (uint64_t)(z >> 63);









z = (unsigned __int128)t0 + (unsigned __int128)(19 * cc);
d[0] = (uint64_t)z;
z = (unsigned __int128)t1 + (z >> 64);
d[1] = (uint64_t)z;
z = (unsigned __int128)t2 + (z >> 64);
d[2] = (uint64_t)z;
d[3] = t3 + (uint64_t)(z >> 64);

#elif BR_UMUL128

uint64_t t0, t1, t2, t3, cc;
unsigned char k;

k = _addcarry_u64(0, a[0], b[0], &t0);
k = _addcarry_u64(k, a[1], b[1], &t1);
k = _addcarry_u64(k, a[2], b[2], &t2);
k = _addcarry_u64(k, a[3], b[3], &t3);
cc = (k << 1) + (t3 >> 63);
t3 &= MASK63;









k = _addcarry_u64(0, t0, 19 * cc, &d[0]);
k = _addcarry_u64(k, t1, 0, &d[1]);
k = _addcarry_u64(k, t2, 0, &d[2]);
(void)_addcarry_u64(k, t3, 0, &d[3]);

#endif
}








static inline void
f255_sub(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
#if BR_INT128








uint64_t t0, t1, t2, t3, t4, cc;
unsigned __int128 z;

z = (unsigned __int128)a[0] - (unsigned __int128)b[0] - 38;
t0 = (uint64_t)z;
cc = -(uint64_t)(z >> 64);
z = (unsigned __int128)a[1] - (unsigned __int128)b[1]
- (unsigned __int128)cc;
t1 = (uint64_t)z;
cc = -(uint64_t)(z >> 64);
z = (unsigned __int128)a[2] - (unsigned __int128)b[2]
- (unsigned __int128)cc;
t2 = (uint64_t)z;
cc = -(uint64_t)(z >> 64);
z = (unsigned __int128)a[3] - (unsigned __int128)b[3]
- (unsigned __int128)cc;
t3 = (uint64_t)z;
t4 = 1 + (uint64_t)(z >> 64);







cc = (38 & -t4) + (19 & -(t3 >> 63));
t3 &= MASK63;
z = (unsigned __int128)t0 + (unsigned __int128)cc;
d[0] = (uint64_t)z;
z = (unsigned __int128)t1 + (z >> 64);
d[1] = (uint64_t)z;
z = (unsigned __int128)t2 + (z >> 64);
d[2] = (uint64_t)z;
d[3] = t3 + (uint64_t)(z >> 64);

#elif BR_UMUL128








uint64_t t0, t1, t2, t3, t4;
unsigned char k;

k = _subborrow_u64(0, a[0], b[0], &t0);
k = _subborrow_u64(k, a[1], b[1], &t1);
k = _subborrow_u64(k, a[2], b[2], &t2);
k = _subborrow_u64(k, a[3], b[3], &t3);
(void)_subborrow_u64(k, 1, 0, &t4);

k = _subborrow_u64(0, t0, 38, &t0);
k = _subborrow_u64(k, t1, 0, &t1);
k = _subborrow_u64(k, t2, 0, &t2);
k = _subborrow_u64(k, t3, 0, &t3);
(void)_subborrow_u64(k, t4, 0, &t4);







t4 = (38 & -t4) + (19 & -(t3 >> 63));
t3 &= MASK63;
k = _addcarry_u64(0, t0, t4, &d[0]);
k = _addcarry_u64(k, t1, 0, &d[1]);
k = _addcarry_u64(k, t2, 0, &d[2]);
(void)_addcarry_u64(k, t3, 0, &d[3]);

#endif
}




static inline void
f255_mul(uint64_t *d, uint64_t *a, uint64_t *b)
{
#if BR_INT128

unsigned __int128 z;
uint64_t t0, t1, t2, t3, t4, t5, t6, t7, th;




z = (unsigned __int128)a[0] * (unsigned __int128)b[0];
t0 = (uint64_t)z;
z = (unsigned __int128)a[0] * (unsigned __int128)b[1] + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)a[0] * (unsigned __int128)b[2] + (z >> 64);
t2 = (uint64_t)z;
z = (unsigned __int128)a[0] * (unsigned __int128)b[3] + (z >> 64);
t3 = (uint64_t)z;
t4 = (uint64_t)(z >> 64);

z = (unsigned __int128)a[1] * (unsigned __int128)b[0]
+ (unsigned __int128)t1;
t1 = (uint64_t)z;
z = (unsigned __int128)a[1] * (unsigned __int128)b[1]
+ (unsigned __int128)t2 + (z >> 64);
t2 = (uint64_t)z;
z = (unsigned __int128)a[1] * (unsigned __int128)b[2]
+ (unsigned __int128)t3 + (z >> 64);
t3 = (uint64_t)z;
z = (unsigned __int128)a[1] * (unsigned __int128)b[3]
+ (unsigned __int128)t4 + (z >> 64);
t4 = (uint64_t)z;
t5 = (uint64_t)(z >> 64);

z = (unsigned __int128)a[2] * (unsigned __int128)b[0]
+ (unsigned __int128)t2;
t2 = (uint64_t)z;
z = (unsigned __int128)a[2] * (unsigned __int128)b[1]
+ (unsigned __int128)t3 + (z >> 64);
t3 = (uint64_t)z;
z = (unsigned __int128)a[2] * (unsigned __int128)b[2]
+ (unsigned __int128)t4 + (z >> 64);
t4 = (uint64_t)z;
z = (unsigned __int128)a[2] * (unsigned __int128)b[3]
+ (unsigned __int128)t5 + (z >> 64);
t5 = (uint64_t)z;
t6 = (uint64_t)(z >> 64);

z = (unsigned __int128)a[3] * (unsigned __int128)b[0]
+ (unsigned __int128)t3;
t3 = (uint64_t)z;
z = (unsigned __int128)a[3] * (unsigned __int128)b[1]
+ (unsigned __int128)t4 + (z >> 64);
t4 = (uint64_t)z;
z = (unsigned __int128)a[3] * (unsigned __int128)b[2]
+ (unsigned __int128)t5 + (z >> 64);
t5 = (uint64_t)z;
z = (unsigned __int128)a[3] * (unsigned __int128)b[3]
+ (unsigned __int128)t6 + (z >> 64);
t6 = (uint64_t)z;
t7 = (uint64_t)(z >> 64);












th = t7 >> 62;
t7 = ((t7 << 1) | (t6 >> 63)) & MASK63;
t6 = (t6 << 1) | (t5 >> 63);
t5 = (t5 << 1) | (t4 >> 63);
t4 = (t4 << 1) | (t3 >> 63);
t3 &= MASK63;





z = (unsigned __int128)t4 * 19;
t4 = (uint64_t)z;
z = (unsigned __int128)t5 * 19 + (z >> 64);
t5 = (uint64_t)z;
z = (unsigned __int128)t6 * 19 + (z >> 64);
t6 = (uint64_t)z;
z = (unsigned __int128)t7 * 19 + (z >> 64);
t7 = (uint64_t)z & MASK63;

th = (361 & -th) + (19 * (uint64_t)(z >> 63));








z = (unsigned __int128)t0 + (unsigned __int128)t4
+ (unsigned __int128)th;
t0 = (uint64_t)z;
z = (unsigned __int128)t1 + (unsigned __int128)t5 + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)t2 + (unsigned __int128)t6 + (z >> 64);
t2 = (uint64_t)z;
z = (unsigned __int128)t3 + (unsigned __int128)t7 + (z >> 64);
t3 = (uint64_t)z & MASK63;
th = (uint64_t)(z >> 63);






z = (unsigned __int128)t0 + (19 * th);
d[0] = (uint64_t)z;
z = (unsigned __int128)t1 + (z >> 64);
d[1] = (uint64_t)z;
z = (unsigned __int128)t2 + (z >> 64);
d[2] = (uint64_t)z;
d[3] = t3 + (uint64_t)(z >> 64);

#elif BR_UMUL128

uint64_t t0, t1, t2, t3, t4, t5, t6, t7, th;
uint64_t h0, h1, h2, h3;
unsigned char k;




t0 = _umul128(a[0], b[0], &h0);
t1 = _umul128(a[0], b[1], &h1);
k = _addcarry_u64(0, t1, h0, &t1);
t2 = _umul128(a[0], b[2], &h2);
k = _addcarry_u64(k, t2, h1, &t2);
t3 = _umul128(a[0], b[3], &h3);
k = _addcarry_u64(k, t3, h2, &t3);
(void)_addcarry_u64(k, h3, 0, &t4);

k = _addcarry_u64(0, _umul128(a[1], b[0], &h0), t1, &t1);
k = _addcarry_u64(k, _umul128(a[1], b[1], &h1), t2, &t2);
k = _addcarry_u64(k, _umul128(a[1], b[2], &h2), t3, &t3);
k = _addcarry_u64(k, _umul128(a[1], b[3], &h3), t4, &t4);
t5 = k;
k = _addcarry_u64(0, t2, h0, &t2);
k = _addcarry_u64(k, t3, h1, &t3);
k = _addcarry_u64(k, t4, h2, &t4);
(void)_addcarry_u64(k, t5, h3, &t5);

k = _addcarry_u64(0, _umul128(a[2], b[0], &h0), t2, &t2);
k = _addcarry_u64(k, _umul128(a[2], b[1], &h1), t3, &t3);
k = _addcarry_u64(k, _umul128(a[2], b[2], &h2), t4, &t4);
k = _addcarry_u64(k, _umul128(a[2], b[3], &h3), t5, &t5);
t6 = k;
k = _addcarry_u64(0, t3, h0, &t3);
k = _addcarry_u64(k, t4, h1, &t4);
k = _addcarry_u64(k, t5, h2, &t5);
(void)_addcarry_u64(k, t6, h3, &t6);

k = _addcarry_u64(0, _umul128(a[3], b[0], &h0), t3, &t3);
k = _addcarry_u64(k, _umul128(a[3], b[1], &h1), t4, &t4);
k = _addcarry_u64(k, _umul128(a[3], b[2], &h2), t5, &t5);
k = _addcarry_u64(k, _umul128(a[3], b[3], &h3), t6, &t6);
t7 = k;
k = _addcarry_u64(0, t4, h0, &t4);
k = _addcarry_u64(k, t5, h1, &t5);
k = _addcarry_u64(k, t6, h2, &t6);
(void)_addcarry_u64(k, t7, h3, &t7);












th = t7 >> 62;
t7 = ((t7 << 1) | (t6 >> 63)) & MASK63;
t6 = (t6 << 1) | (t5 >> 63);
t5 = (t5 << 1) | (t4 >> 63);
t4 = (t4 << 1) | (t3 >> 63);
t3 &= MASK63;





t4 = _umul128(t4, 19, &h0);
t5 = _umul128(t5, 19, &h1);
t6 = _umul128(t6, 19, &h2);
t7 = _umul128(t7, 19, &h3);
k = _addcarry_u64(0, t5, h0, &t5);
k = _addcarry_u64(k, t6, h1, &t6);
k = _addcarry_u64(k, t7, h2, &t7);
(void)_addcarry_u64(k, h3, 0, &h3);
th = (361 & -th) + (19 * ((h3 << 1) + (t7 >> 63)));
t7 &= MASK63;








k = _addcarry_u64(0, t0, t4, &t0);
k = _addcarry_u64(k, t1, t5, &t1);
k = _addcarry_u64(k, t2, t6, &t2);
k = _addcarry_u64(k, t3, t7, &t3);
t4 = k;
k = _addcarry_u64(0, t0, th, &t0);
k = _addcarry_u64(k, t1, 0, &t1);
k = _addcarry_u64(k, t2, 0, &t2);
k = _addcarry_u64(k, t3, 0, &t3);
(void)_addcarry_u64(k, t4, 0, &t4);

th = (t4 << 1) + (t3 >> 63);
t3 &= MASK63;






k = _addcarry_u64(0, t0, 19 * th, &d[0]);
k = _addcarry_u64(k, t1, 0, &d[1]);
k = _addcarry_u64(k, t2, 0, &d[2]);
(void)_addcarry_u64(k, t3, 0, &d[3]);

#endif
}




static inline void
f255_mul_a24(uint64_t *d, const uint64_t *a)
{
#if BR_INT128

uint64_t t0, t1, t2, t3;
unsigned __int128 z;

z = (unsigned __int128)a[0] * 121665;
t0 = (uint64_t)z;
z = (unsigned __int128)a[1] * 121665 + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)a[2] * 121665 + (z >> 64);
t2 = (uint64_t)z;
z = (unsigned __int128)a[3] * 121665 + (z >> 64);
t3 = (uint64_t)z & MASK63;

z = (unsigned __int128)t0 + (19 * (uint64_t)(z >> 63));
t0 = (uint64_t)z;
z = (unsigned __int128)t1 + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)t2 + (z >> 64);
t2 = (uint64_t)z;
t3 = t3 + (uint64_t)(z >> 64);

z = (unsigned __int128)t0 + (19 & -(t3 >> 63));
d[0] = (uint64_t)z;
z = (unsigned __int128)t1 + (z >> 64);
d[1] = (uint64_t)z;
z = (unsigned __int128)t2 + (z >> 64);
d[2] = (uint64_t)z;
d[3] = (t3 & MASK63) + (uint64_t)(z >> 64);

#elif BR_UMUL128

uint64_t t0, t1, t2, t3, t4, h0, h1, h2, h3;
unsigned char k;

t0 = _umul128(a[0], 121665, &h0);
t1 = _umul128(a[1], 121665, &h1);
k = _addcarry_u64(0, t1, h0, &t1);
t2 = _umul128(a[2], 121665, &h2);
k = _addcarry_u64(k, t2, h1, &t2);
t3 = _umul128(a[3], 121665, &h3);
k = _addcarry_u64(k, t3, h2, &t3);
(void)_addcarry_u64(k, h3, 0, &t4);

t4 = (t4 << 1) + (t3 >> 63);
t3 &= MASK63;
k = _addcarry_u64(0, t0, 19 * t4, &t0);
k = _addcarry_u64(k, t1, 0, &t1);
k = _addcarry_u64(k, t2, 0, &t2);
(void)_addcarry_u64(k, t3, 0, &t3);

t4 = 19 & -(t3 >> 63);
t3 &= MASK63;
k = _addcarry_u64(0, t0, t4, &d[0]);
k = _addcarry_u64(k, t1, 0, &d[1]);
k = _addcarry_u64(k, t2, 0, &d[2]);
(void)_addcarry_u64(k, t3, 0, &d[3]);

#endif
}




static inline void
f255_final_reduce(uint64_t *a)
{
#if BR_INT128

uint64_t t0, t1, t2, t3, m;
unsigned __int128 z;







z = (unsigned __int128)a[0] + 19;
t0 = (uint64_t)z;
z = (unsigned __int128)a[1] + (z >> 64);
t1 = (uint64_t)z;
z = (unsigned __int128)a[2] + (z >> 64);
t2 = (uint64_t)z;
t3 = a[3] + (uint64_t)(z >> 64);

m = -(t3 >> 63);
t3 &= MASK63;
a[0] ^= m & (a[0] ^ t0);
a[1] ^= m & (a[1] ^ t1);
a[2] ^= m & (a[2] ^ t2);
a[3] ^= m & (a[3] ^ t3);

#elif BR_UMUL128

uint64_t t0, t1, t2, t3, m;
unsigned char k;







k = _addcarry_u64(0, a[0], 19, &t0);
k = _addcarry_u64(k, a[1], 0, &t1);
k = _addcarry_u64(k, a[2], 0, &t2);
(void)_addcarry_u64(k, a[3], 0, &t3);

m = -(t3 >> 63);
t3 &= MASK63;
a[0] ^= m & (a[0] ^ t0);
a[1] ^= m & (a[1] ^ t1);
a[2] ^= m & (a[2] ^ t2);
a[3] ^= m & (a[3] ^ t3);

#endif
}

static uint32_t
api_mul(unsigned char *G, size_t Glen,
const unsigned char *kb, size_t kblen, int curve)
{
unsigned char k[32];
uint64_t x1[4], x2[4], z2[4], x3[4], z3[4];
uint32_t swap;
int i;

(void)curve;





if (Glen != 32 || kblen > 32) {
return 0;
}





x1[0] = br_dec64le(&G[ 0]);
x1[1] = br_dec64le(&G[ 8]);
x1[2] = br_dec64le(&G[16]);
x1[3] = br_dec64le(&G[24]) & MASK63;






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
uint64_t a[4], aa[4], b[4], bb[4], e[4];
uint64_t c[4], d[4], da[4], cb[4];
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


const br_ec_impl br_ec_c25519_m64 = {
(uint32_t)0x20000000,
&api_generator,
&api_order,
&api_xoff,
&api_mul,
&api_mulgen,
&api_muladd
};


const br_ec_impl *
br_ec_c25519_m64_get(void)
{
return &br_ec_c25519_m64;
}

#else


const br_ec_impl *
br_ec_c25519_m64_get(void)
{
return 0;
}

#endif
