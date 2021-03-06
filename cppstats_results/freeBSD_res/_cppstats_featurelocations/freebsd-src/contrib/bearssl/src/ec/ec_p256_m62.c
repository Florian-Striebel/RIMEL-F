























#include "inner.h"

#if BR_INT128 || BR_UMUL128

#if BR_UMUL128
#include <intrin.h>
#endif

static const unsigned char P256_G[] = {
0x04, 0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8,
0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2, 0x77, 0x03, 0x7D,
0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8,
0x98, 0xC2, 0x96, 0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F,
0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16, 0x2B,
0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6, 0x40,
0x68, 0x37, 0xBF, 0x51, 0xF5
};

static const unsigned char P256_N[] = {
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD,
0xA7, 0x17, 0x9E, 0x84, 0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63,
0x25, 0x51
};

static const unsigned char *
api_generator(int curve, size_t *len)
{
(void)curve;
*len = sizeof P256_G;
return P256_G;
}

static const unsigned char *
api_order(int curve, size_t *len)
{
(void)curve;
*len = sizeof P256_N;
return P256_N;
}

static size_t
api_xoff(int curve, size_t *len)
{
(void)curve;
*len = 32;
return 1;
}











#define BIT(n) ((uint64_t)1 << (n))
#define MASK48 (BIT(48) - BIT(0))
#define MASK52 (BIT(52) - BIT(0))


static const uint64_t F256_R[] = {
0x0000000000010, 0xF000000000000, 0xFFFFFFFFFFFFF,
0xFFEFFFFFFFFFF, 0x00000000FFFFF
};



static const uint64_t P256_B_MONTY[] = {
0xDF6229C4BDDFD, 0xCA8843090D89C, 0x212ED6ACF005C,
0x83415A220ABF7, 0x0C30061DD4874
};






static inline void
f256_add(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
d[0] = a[0] + b[0];
d[1] = a[1] + b[1];
d[2] = a[2] + b[2];
d[3] = a[3] + b[3];
d[4] = a[4] + b[4];
}






static inline void
f256_partial_reduce(uint64_t *a)
{
uint64_t w, cc, s;




w = a[0];
a[0] = w & MASK52;
cc = w >> 52;
w = a[1] + cc;
a[1] = w & MASK52;
cc = w >> 52;
w = a[2] + cc;
a[2] = w & MASK52;
cc = w >> 52;
w = a[3] + cc;
a[3] = w & MASK52;
cc = w >> 52;
a[4] += cc;

s = a[4] >> 48;
a[0] += s;
w = a[1] - (s << 44);
a[1] = w & MASK52;
cc = -(w >> 52) & 0xFFF;
w = a[2] - cc;
a[2] = w & MASK52;
cc = w >> 63;
w = a[3] - cc - (s << 36);
a[3] = w & MASK52;
cc = w >> 63;
w = a[4] & MASK48;
a[4] = w + (s << 16) - cc;
}







static inline void
f256_sub(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
uint64_t t[5], w, s, cc;

















w = a[0] - b[0] - BIT(13);
t[0] = w & MASK52;
cc = w >> 52;
cc |= -(cc & BIT(11));
w = a[1] - b[1] + cc;
t[1] = w & MASK52;
cc = w >> 52;
cc |= -(cc & BIT(11));
w = a[2] - b[2] + cc;
t[2] = (w & MASK52) + BIT(5);
cc = w >> 52;
cc |= -(cc & BIT(11));
w = a[3] - b[3] + cc;
t[3] = (w & MASK52) + BIT(49);
cc = w >> 52;
cc |= -(cc & BIT(11));
t[4] = (BIT(61) - BIT(29)) + a[4] - b[4] + cc;















s = t[4] >> 48;

d[0] = t[0] + s;
w = t[1] - (s << 44);
d[1] = w & MASK52;
cc = -(w >> 52) & 0xFFF;
w = t[2] - cc;
cc = w >> 63;
d[2] = w + (cc << 52);
w = t[3] - cc - (s << 36);
cc = w >> 63;
d[3] = w + (cc << 52);
d[4] = (t[4] & MASK48) + (s << 16) - cc;






}






static void
f256_montymul(uint64_t *d, const uint64_t *a, const uint64_t *b)
{
#if BR_INT128

int i;
uint64_t t[5];

t[0] = 0;
t[1] = 0;
t[2] = 0;
t[3] = 0;
t[4] = 0;
for (i = 0; i < 5; i ++) {
uint64_t x, f, cc, w, s;
unsigned __int128 z;











x = a[i];
z = (unsigned __int128)b[0] * (unsigned __int128)x
+ (unsigned __int128)t[0];
f = (uint64_t)z & MASK52;
cc = (uint64_t)(z >> 52);
z = (unsigned __int128)b[1] * (unsigned __int128)x
+ (unsigned __int128)t[1] + cc
+ ((unsigned __int128)f << 44);
t[0] = (uint64_t)z & MASK52;
cc = (uint64_t)(z >> 52);
z = (unsigned __int128)b[2] * (unsigned __int128)x
+ (unsigned __int128)t[2] + cc;
t[1] = (uint64_t)z & MASK52;
cc = (uint64_t)(z >> 52);
z = (unsigned __int128)b[3] * (unsigned __int128)x
+ (unsigned __int128)t[3] + cc
+ ((unsigned __int128)f << 36);
t[2] = (uint64_t)z & MASK52;
cc = (uint64_t)(z >> 52);
z = (unsigned __int128)b[4] * (unsigned __int128)x
+ (unsigned __int128)t[4] + cc
+ ((unsigned __int128)f << 48)
- ((unsigned __int128)f << 16);
t[3] = (uint64_t)z & MASK52;
t[4] = (uint64_t)(z >> 52);






s = t[4] >> 48;
t[0] += s;
w = t[1] - (s << 44);
t[1] = w & MASK52;
cc = -(w >> 52) & 0xFFF;
w = t[2] - cc;
t[2] = w & MASK52;
cc = w >> 63;
w = t[3] - cc - (s << 36);
t[3] = w & MASK52;
cc = w >> 63;
w = t[4] & MASK48;
t[4] = w + (s << 16) - cc;





}

d[0] = t[0];
d[1] = t[1];
d[2] = t[2];
d[3] = t[3];
d[4] = t[4];

#elif BR_UMUL128

int i;
uint64_t t[5];

t[0] = 0;
t[1] = 0;
t[2] = 0;
t[3] = 0;
t[4] = 0;
for (i = 0; i < 5; i ++) {
uint64_t x, f, cc, w, s, zh, zl;
unsigned char k;











x = a[i];
zl = _umul128(b[0], x, &zh);
k = _addcarry_u64(0, t[0], zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
f = zl & MASK52;
cc = (zl >> 52) | (zh << 12);

zl = _umul128(b[1], x, &zh);
k = _addcarry_u64(0, t[1], zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, cc, zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, f << 44, zl, &zl);
(void)_addcarry_u64(k, f >> 20, zh, &zh);
t[0] = zl & MASK52;
cc = (zl >> 52) | (zh << 12);

zl = _umul128(b[2], x, &zh);
k = _addcarry_u64(0, t[2], zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, cc, zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
t[1] = zl & MASK52;
cc = (zl >> 52) | (zh << 12);

zl = _umul128(b[3], x, &zh);
k = _addcarry_u64(0, t[3], zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, cc, zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, f << 36, zl, &zl);
(void)_addcarry_u64(k, f >> 28, zh, &zh);
t[2] = zl & MASK52;
cc = (zl >> 52) | (zh << 12);

zl = _umul128(b[4], x, &zh);
k = _addcarry_u64(0, t[4], zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, cc, zl, &zl);
(void)_addcarry_u64(k, 0, zh, &zh);
k = _addcarry_u64(0, f << 48, zl, &zl);
(void)_addcarry_u64(k, f >> 16, zh, &zh);
k = _subborrow_u64(0, zl, f << 16, &zl);
(void)_subborrow_u64(k, zh, f >> 48, &zh);
t[3] = zl & MASK52;
t[4] = (zl >> 52) | (zh << 12);






s = t[4] >> 48;
t[0] += s;
w = t[1] - (s << 44);
t[1] = w & MASK52;
cc = -(w >> 52) & 0xFFF;
w = t[2] - cc;
t[2] = w & MASK52;
cc = w >> 63;
w = t[3] - cc - (s << 36);
t[3] = w & MASK52;
cc = w >> 63;
w = t[4] & MASK48;
t[4] = w + (s << 16) - cc;





}

d[0] = t[0];
d[1] = t[1];
d[2] = t[2];
d[3] = t[3];
d[4] = t[4];

#endif
}






static inline void
f256_montysquare(uint64_t *d, const uint64_t *a)
{
f256_montymul(d, a, a);
}




static void
f256_tomonty(uint64_t *d, const uint64_t *a)
{






static const uint64_t R2[] = {
0x0000000000300, 0xFFFFFFFF00000, 0xFFFFEFFFFFFFB,
0xFDFFFFFFFFFFF, 0x0000004FFFFFF
};

f256_montymul(d, a, R2);
}




static void
f256_frommonty(uint64_t *d, const uint64_t *a)
{



static const uint64_t one[] = { 1, 0, 0, 0, 0 };

f256_montymul(d, a, one);
}





static void
f256_invert(uint64_t *d, const uint64_t *a)
{














uint64_t r[5], t[5];
int i;

memcpy(t, a, sizeof t);
for (i = 0; i < 30; i ++) {
f256_montysquare(t, t);
f256_montymul(t, t, a);
}

memcpy(r, t, sizeof t);
for (i = 224; i >= 0; i --) {
f256_montysquare(r, r);
switch (i) {
case 0:
case 2:
case 192:
case 224:
f256_montymul(r, r, a);
break;
case 3:
case 34:
case 65:
f256_montymul(r, r, t);
break;
}
}
memcpy(d, r, sizeof r);
}







static inline void
f256_final_reduce(uint64_t *a)
{
uint64_t r[5], t[5], w, cc;
int i;




cc = 0;
for (i = 0; i < 5; i ++) {
w = a[i] + cc;
r[i] = w & MASK52;
cc = w >> 52;
}











w = r[0] + 1;
t[0] = w & MASK52;
cc = w >> 52;
w = r[1] + cc;
t[1] = w & MASK52;
cc = w >> 52;
w = r[2] + cc;
t[2] = w & MASK52;
cc = w >> 52;
w = r[3] + cc;
t[3] = w & MASK52;
cc = w >> 52;
t[4] = r[4] + cc + BIT(16);





w = t[1] - BIT(44);
t[1] = w & MASK52;
cc = w >> 63;
w = t[2] - cc;
t[2] = w & MASK52;
cc = w >> 63;
w = t[3] - BIT(36);
t[3] = w & MASK52;
cc = w >> 63;
t[4] -= cc;






cc = -(t[4] >> 48);
t[4] &= MASK48;
for (i = 0; i < 5; i ++) {
a[i] = r[i] ^ (cc & (r[i] ^ t[i]));
}
}








typedef struct {
uint64_t x[5];
uint64_t y[5];
} p256_affine;

typedef struct {
uint64_t x[5];
uint64_t y[5];
uint64_t z[5];
} p256_jacobian;




static void
f256_decode(uint64_t *a, const unsigned char *buf)
{
uint64_t w0, w1, w2, w3;

w3 = br_dec64be(buf + 0);
w2 = br_dec64be(buf + 8);
w1 = br_dec64be(buf + 16);
w0 = br_dec64be(buf + 24);
a[0] = w0 & MASK52;
a[1] = ((w0 >> 52) | (w1 << 12)) & MASK52;
a[2] = ((w1 >> 40) | (w2 << 24)) & MASK52;
a[3] = ((w2 >> 28) | (w3 << 36)) & MASK52;
a[4] = w3 >> 16;
}





static void
f256_encode(unsigned char *buf, const uint64_t *a)
{
uint64_t w0, w1, w2, w3;

w0 = a[0] | (a[1] << 52);
w1 = (a[1] >> 12) | (a[2] << 40);
w2 = (a[2] >> 24) | (a[3] << 28);
w3 = (a[3] >> 36) | (a[4] << 16);
br_enc64be(buf + 0, w3);
br_enc64be(buf + 8, w2);
br_enc64be(buf + 16, w1);
br_enc64be(buf + 24, w0);
}









static uint32_t
point_decode(p256_jacobian *P, const unsigned char *buf)
{
uint64_t x[5], y[5], t[5], x3[5], tt;
uint32_t r;




r = EQ(buf[0], 0x04);





f256_decode(x, buf + 1);
f256_decode(y, buf + 33);
f256_tomonty(x, x);
f256_tomonty(y, y);







f256_montysquare(t, y);
f256_montysquare(x3, x);
f256_montymul(x3, x3, x);
f256_sub(t, t, x3);
f256_add(t, t, x);
f256_add(t, t, x);
f256_add(t, t, x);
f256_sub(t, t, P256_B_MONTY);
f256_final_reduce(t);
tt = t[0] | t[1] | t[2] | t[3] | t[4];
r &= EQ((uint32_t)(tt | (tt >> 32)), 0);





memcpy(P->x, x, sizeof x);
memcpy(P->y, y, sizeof y);
memcpy(P->z, F256_R, sizeof F256_R);
return r;
}











static uint32_t
point_encode(unsigned char *buf, const p256_jacobian *P)
{
uint64_t t1[5], t2[5], z;


f256_invert(t2, P->z);
f256_montysquare(t1, t2);
f256_montymul(t2, t2, t1);


f256_montymul(t1, P->x, t1);
f256_montymul(t2, P->y, t2);



f256_frommonty(t1, t1);
f256_frommonty(t2, t2);
f256_final_reduce(t1);
f256_final_reduce(t2);


buf[0] = 0x04;
f256_encode(buf + 1, t1);
f256_encode(buf + 33, t2);


z = P->z[0] | P->z[1] | P->z[2] | P->z[3] | P->z[4];
return NEQ((uint32_t)(z | z >> 32), 0);
}







static void
p256_double(p256_jacobian *P)
{















uint64_t t1[5], t2[5], t3[5], t4[5];




f256_montysquare(t1, P->z);




f256_add(t2, P->x, t1);
f256_sub(t1, P->x, t1);




f256_montymul(t3, t1, t2);
f256_add(t1, t3, t3);
f256_add(t1, t3, t1);




f256_montysquare(t3, P->y);
f256_add(t3, t3, t3);
f256_montymul(t2, P->x, t3);
f256_add(t2, t2, t2);




f256_montysquare(P->x, t1);
f256_sub(P->x, P->x, t2);
f256_sub(P->x, P->x, t2);




f256_montymul(t4, P->y, P->z);
f256_add(P->z, t4, t4);
f256_partial_reduce(P->z);





f256_sub(t2, t2, P->x);
f256_montymul(P->y, t1, t2);
f256_montysquare(t4, t3);
f256_add(t4, t4, t4);
f256_sub(P->y, P->y, t4);
}


































static uint32_t
p256_add(p256_jacobian *P1, const p256_jacobian *P2)
{













uint64_t t1[5], t2[5], t3[5], t4[5], t5[5], t6[5], t7[5], tt;
uint32_t ret;




f256_montysquare(t3, P2->z);
f256_montymul(t1, P1->x, t3);
f256_montymul(t4, P2->z, t3);
f256_montymul(t3, P1->y, t4);




f256_montysquare(t4, P1->z);
f256_montymul(t2, P2->x, t4);
f256_montymul(t5, P1->z, t4);
f256_montymul(t4, P2->y, t5);






f256_sub(t2, t2, t1);
f256_sub(t4, t4, t3);
f256_final_reduce(t4);
tt = t4[0] | t4[1] | t4[2] | t4[3] | t4[4];
ret = (uint32_t)(tt | (tt >> 32));
ret = (ret | -ret) >> 31;




f256_montysquare(t7, t2);
f256_montymul(t6, t1, t7);
f256_montymul(t5, t7, t2);




f256_montysquare(P1->x, t4);
f256_sub(P1->x, P1->x, t5);
f256_sub(P1->x, P1->x, t6);
f256_sub(P1->x, P1->x, t6);




f256_sub(t6, t6, P1->x);
f256_montymul(P1->y, t4, t6);
f256_montymul(t1, t5, t3);
f256_sub(P1->y, P1->y, t1);




f256_montymul(t1, P1->z, P2->z);
f256_montymul(P1->z, t1, t2);

return ret;
}


































static uint32_t
p256_add_mixed(p256_jacobian *P1, const p256_affine *P2)
{













uint64_t t1[5], t2[5], t3[5], t4[5], t5[5], t6[5], t7[5], tt;
uint32_t ret;




memcpy(t1, P1->x, sizeof t1);
memcpy(t3, P1->y, sizeof t3);




f256_montysquare(t4, P1->z);
f256_montymul(t2, P2->x, t4);
f256_montymul(t5, P1->z, t4);
f256_montymul(t4, P2->y, t5);






f256_sub(t2, t2, t1);
f256_sub(t4, t4, t3);
f256_final_reduce(t4);
tt = t4[0] | t4[1] | t4[2] | t4[3] | t4[4];
ret = (uint32_t)(tt | (tt >> 32));
ret = (ret | -ret) >> 31;




f256_montysquare(t7, t2);
f256_montymul(t6, t1, t7);
f256_montymul(t5, t7, t2);




f256_montysquare(P1->x, t4);
f256_sub(P1->x, P1->x, t5);
f256_sub(P1->x, P1->x, t6);
f256_sub(P1->x, P1->x, t6);




f256_sub(t6, t6, P1->x);
f256_montymul(P1->y, t4, t6);
f256_montymul(t1, t5, t3);
f256_sub(P1->y, P1->y, t1);




f256_montymul(P1->z, P1->z, t2);

return ret;
}

#if 0








static uint32_t
p256_add_complete_mixed(p256_jacobian *P1, const p256_affine *P2)
{













































uint64_t t1[5], t2[5], t3[5], t4[5], t5[5], t6[5], t7[5], tt, zz;
int i;




zz = P1->z[0] | P1->z[1] | P1->z[2] | P1->z[3] | P1->z[4];
zz = ((zz | -zz) >> 63) - (uint64_t)1;




memcpy(t1, P1->x, sizeof t1);
memcpy(t3, P1->y, sizeof t3);




f256_montysquare(t4, P1->z);
f256_montymul(t2, P2->x, t4);
f256_montymul(t5, P1->z, t4);
f256_montymul(t4, P2->y, t5);





f256_sub(t2, t2, t1);
f256_sub(t4, t4, t3);





f256_final_reduce(t2);
f256_final_reduce(t4);
tt = t2[0] | t2[1] | t2[2] | t2[3] | t2[4]
| t4[0] | t4[1] | t4[2] | t4[3] | t4[4];
tt = ((tt | -tt) >> 63) - (uint64_t)1;




f256_montysquare(t7, t2);
f256_montymul(t6, t1, t7);
f256_montymul(t5, t7, t2);




f256_montysquare(P1->x, t4);
f256_sub(P1->x, P1->x, t5);
f256_sub(P1->x, P1->x, t6);
f256_sub(P1->x, P1->x, t6);




f256_sub(t6, t6, P1->x);
f256_montymul(P1->y, t4, t6);
f256_montymul(t1, t5, t3);
f256_sub(P1->y, P1->y, t1);




f256_montymul(P1->z, P1->z, t2);








f256_add(t1, P2->y, P2->y);
f256_partial_reduce(t1);




f256_montysquare(t2, P2->y);
f256_add(t2, t2, t2);
f256_add(t3, t2, t2);
f256_montymul(t3, P2->x, t3);




f256_montysquare(t4, P2->x);
f256_sub(t4, t4, F256_R);
f256_add(t5, t4, t4);
f256_add(t4, t4, t5);




f256_montysquare(t5, t4);
f256_sub(t5, t3);
f256_sub(t5, t3);




f256_sub(t6, t3, t5);
f256_montymul(t6, t6, t4);
f256_montysquare(t7, t2);
f256_sub(t6, t6, t7);
f256_sub(t6, t6, t7);





for (i = 0; i < 5; i ++) {
P1->x[i] |= tt & t5[i];
P1->y[i] |= tt & t6[i];
P1->z[i] |= tt & t1[i];
}






for (i = 0; i < 5; i ++) {
P1->x[i] ^= zz & (P1->x[i] ^ P2->x[i]);
P1->y[i] ^= zz & (P1->y[i] ^ P2->y[i]);
P1->z[i] ^= zz & (P1->z[i] ^ F256_R[i]);
}
}
#endif










static void
point_mul_inner(p256_jacobian *R, const p256_affine *W,
const unsigned char *k, size_t klen)
{
p256_jacobian Q;
uint32_t qz;

memset(&Q, 0, sizeof Q);
qz = 1;
while (klen -- > 0) {
int i;
unsigned bk;

bk = *k ++;
for (i = 0; i < 2; i ++) {
uint32_t bits;
uint32_t bnz;
p256_affine T;
p256_jacobian U;
uint32_t n;
int j;
uint64_t m;

p256_double(&Q);
p256_double(&Q);
p256_double(&Q);
p256_double(&Q);
bits = (bk >> 4) & 0x0F;
bnz = NEQ(bits, 0);







memset(&T, 0, sizeof T);
for (n = 0; n < 15; n ++) {
m = -(uint64_t)EQ(bits, n + 1);
T.x[0] |= m & W[n].x[0];
T.x[1] |= m & W[n].x[1];
T.x[2] |= m & W[n].x[2];
T.x[3] |= m & W[n].x[3];
T.x[4] |= m & W[n].x[4];
T.y[0] |= m & W[n].y[0];
T.y[1] |= m & W[n].y[1];
T.y[2] |= m & W[n].y[2];
T.y[3] |= m & W[n].y[3];
T.y[4] |= m & W[n].y[4];
}

U = Q;
p256_add_mixed(&U, &T);





m = -(uint64_t)(bnz & qz);
for (j = 0; j < 5; j ++) {
Q.x[j] ^= m & (Q.x[j] ^ T.x[j]);
Q.y[j] ^= m & (Q.y[j] ^ T.y[j]);
Q.z[j] ^= m & (Q.z[j] ^ F256_R[j]);
}
CCOPY(bnz & ~qz, &Q, &U, sizeof Q);
qz &= ~bnz;
bk <<= 4;
}
}
*R = Q;
}











static void
window_to_affine(p256_affine *aff, p256_jacobian *jac, int num)
{























































uint64_t z[16][5];
int i, k, s;
#define zt (z[15])
#define zu (z[14])
#define zv (z[13])






for (i = 0; (i + 1) < num; i += 2) {
memcpy(zt, jac[i].z, sizeof zt);
memcpy(jac[i].z, jac[i + 1].z, sizeof zt);
memcpy(jac[i + 1].z, zt, sizeof zt);
f256_montymul(z[i >> 1], jac[i].z, jac[i + 1].z);
}
if ((num & 1) != 0) {
memcpy(z[num >> 1], jac[num - 1].z, sizeof zt);
memcpy(jac[num - 1].z, F256_R, sizeof F256_R);
}






for (k = 1, s = 2; s < num; k ++, s <<= 1) {
int n;

for (i = 0; i < num; i ++) {
f256_montymul(jac[i].z, jac[i].z, z[(i >> k) ^ 1]);
}
n = (num + s - 1) >> k;
for (i = 0; i < (n >> 1); i ++) {
f256_montymul(z[i], z[i << 1], z[(i << 1) + 1]);
}
if ((n & 1) != 0) {
memmove(z[n >> 1], z[n], sizeof zt);
}
}




f256_invert(zt, z[0]);
for (i = 0; i < num; i ++) {
f256_montymul(zv, jac[i].z, zt);
f256_montysquare(zu, zv);
f256_montymul(zv, zv, zu);
f256_montymul(aff[i].x, jac[i].x, zu);
f256_montymul(aff[i].y, jac[i].y, zv);
}
}










static void
p256_mul(p256_jacobian *P, const unsigned char *k, size_t klen)
{
union {
p256_affine aff[15];
p256_jacobian jac[15];
} window;
int i;




window.jac[0] = *P;
for (i = 2; i < 16; i ++) {
window.jac[i - 1] = window.jac[(i >> 1) - 1];
if ((i & 1) == 0) {
p256_double(&window.jac[i - 1]);
} else {
p256_add(&window.jac[i - 1], &window.jac[i >> 1]);
}
}





window_to_affine(window.aff, window.jac, 15);




point_mul_inner(P, window.aff, k, klen);
}





static const p256_affine P256_Gwin[] = {
{
{ 0x30D418A9143C1, 0xC4FEDB60179E7, 0x62251075BA95F,
0x5C669FB732B77, 0x08905F76B5375 },
{ 0x5357CE95560A8, 0x43A19E45CDDF2, 0x21F3258B4AB8E,
0xD8552E88688DD, 0x0571FF18A5885 }
},
{
{ 0x46D410DDD64DF, 0x0B433827D8500, 0x1490D9AA6AE3C,
0xA3A832205038D, 0x06BB32E52DCF3 },
{ 0x48D361BEE1A57, 0xB7B236FF82F36, 0x042DBE152CD7C,
0xA3AA9A8FB0E92, 0x08C577517A5B8 }
},
{
{ 0x3F904EEBC1272, 0x9E87D81FBFFAC, 0xCBBC98B027F84,
0x47E46AD77DD87, 0x06936A3FD6FF7 },
{ 0x5C1FC983A7EBD, 0xC3861FE1AB04C, 0x2EE98E583E47A,
0xC06A88208311A, 0x05F06A2AB587C }
},
{
{ 0xB50D46918DCC5, 0xD7623C17374B0, 0x100AF24650A6E,
0x76ABCDAACACE8, 0x077362F591B01 },
{ 0xF24CE4CBABA68, 0x17AD6F4472D96, 0xDDD22E1762847,
0x862EB6C36DEE5, 0x04B14C39CC5AB }
},
{
{ 0x8AAEC45C61F5C, 0x9D4B9537DBE1B, 0x76C20C90EC649,
0x3C7D41CB5AAD0, 0x0907960649052 },
{ 0x9B4AE7BA4F107, 0xF75EB882BEB30, 0x7A1F6873C568E,
0x915C540A9877E, 0x03A076BB9DD1E }
},
{
{ 0x47373E77664A1, 0xF246CEE3E4039, 0x17A3AD55AE744,
0x673C50A961A5B, 0x03074B5964213 },
{ 0x6220D377E44BA, 0x30DFF14B593D3, 0x639F11299C2B5,
0x75F5424D44CEF, 0x04C9916DEA07F }
},
{
{ 0x354EA0173B4F1, 0x3C23C00F70746, 0x23BB082BD2021,
0xE03E43EAAB50C, 0x03BA5119D3123 },
{ 0xD0303F5B9D4DE, 0x17DA67BDD2847, 0xC941956742F2F,
0x8670F933BDC77, 0x0AEDD9164E240 }
},
{
{ 0x4CD19499A78FB, 0x4BF9B345527F1, 0x2CFC6B462AB5C,
0x30CDF90F02AF0, 0x0763891F62652 },
{ 0xA3A9532D49775, 0xD7F9EBA15F59D, 0x60BBF021E3327,
0xF75C23C7B84BE, 0x06EC12F2C706D }
},
{
{ 0x6E8F264E20E8E, 0xC79A7A84175C9, 0xC8EB00ABE6BFE,
0x16A4CC09C0444, 0x005B3081D0C4E },
{ 0x777AA45F33140, 0xDCE5D45E31EB7, 0xB12F1A56AF7BE,
0xF9B2B6E019A88, 0x086659CDFD835 }
},
{
{ 0xDBD19DC21EC8C, 0x94FCF81392C18, 0x250B4998F9868,
0x28EB37D2CD648, 0x0C61C947E4B34 },
{ 0x407880DD9E767, 0x0C83FBE080C2B, 0x9BE5D2C43A899,
0xAB4EF7D2D6577, 0x08719A555B3B4 }
},
{
{ 0x260A6245E4043, 0x53E7FDFE0EA7D, 0xAC1AB59DE4079,
0x072EFF3A4158D, 0x0E7090F1949C9 },
{ 0x85612B944E886, 0xE857F61C81A76, 0xAD643D250F939,
0x88DAC0DAA891E, 0x089300244125B }
},
{
{ 0x1AA7D26977684, 0x58A345A3304B7, 0x37385EABDEDEF,
0x155E409D29DEE, 0x0EE1DF780B83E },
{ 0x12D91CBB5B437, 0x65A8956370CAC, 0xDE6D66170ED2F,
0xAC9B8228CFA8A, 0x0FF57C95C3238 }
},
{
{ 0x25634B2ED7097, 0x9156FD30DCCC4, 0x9E98110E35676,
0x7594CBCD43F55, 0x038477ACC395B },
{ 0x2B90C00EE17FF, 0xF842ED2E33575, 0x1F5BC16874838,
0x7968CD06422BD, 0x0BC0876AB9E7B }
},
{
{ 0xA35BB0CF664AF, 0x68F9707E3A242, 0x832660126E48F,
0x72D2717BF54C6, 0x0AAE7333ED12C },
{ 0x2DB7995D586B1, 0xE732237C227B5, 0x65E7DBBE29569,
0xBBBD8E4193E2A, 0x052706DC3EAA1 }
},
{
{ 0xD8B7BC60055BE, 0xD76E27E4B72BC, 0x81937003CC23E,
0xA090E337424E4, 0x02AA0E43EAD3D },
{ 0x524F6383C45D2, 0x422A41B2540B8, 0x8A4797D766355,
0xDF444EFA6DE77, 0x0042170A9079A }
},
};










static void
p256_mulgen(p256_jacobian *P, const unsigned char *k, size_t klen)
{
point_mul_inner(P, P256_Gwin, k, klen);
}










static uint32_t
check_scalar(const unsigned char *k, size_t klen)
{
uint32_t z;
int32_t c;
size_t u;

if (klen > 32) {
return 0;
}
z = 0;
for (u = 0; u < klen; u ++) {
z |= k[u];
}
if (klen == 32) {
c = 0;
for (u = 0; u < klen; u ++) {
c |= -(int32_t)EQ0(c) & CMP(k[u], P256_N[u]);
}
} else {
c = -1;
}
return NEQ(z, 0) & LT0(c);
}

static uint32_t
api_mul(unsigned char *G, size_t Glen,
const unsigned char *k, size_t klen, int curve)
{
uint32_t r;
p256_jacobian P;

(void)curve;
if (Glen != 65) {
return 0;
}
r = check_scalar(k, klen);
r &= point_decode(&P, G);
p256_mul(&P, k, klen);
r &= point_encode(G, &P);
return r;
}

static size_t
api_mulgen(unsigned char *R,
const unsigned char *k, size_t klen, int curve)
{
p256_jacobian P;

(void)curve;
p256_mulgen(&P, k, klen);
point_encode(R, &P);
return 65;
}

static uint32_t
api_muladd(unsigned char *A, const unsigned char *B, size_t len,
const unsigned char *x, size_t xlen,
const unsigned char *y, size_t ylen, int curve)
{




























p256_jacobian P, Q;
uint32_t r, t, s;
uint64_t z;

(void)curve;
if (len != 65) {
return 0;
}
r = point_decode(&P, A);
p256_mul(&P, x, xlen);
if (B == NULL) {
p256_mulgen(&Q, y, ylen);
} else {
r &= point_decode(&Q, B);
p256_mul(&Q, y, ylen);
}




t = p256_add(&P, &Q);
f256_final_reduce(P.z);
z = P.z[0] | P.z[1] | P.z[2] | P.z[3] | P.z[4];
s = EQ((uint32_t)(z | (z >> 32)), 0);
p256_double(&Q);










CCOPY(s & ~t, &P, &Q, sizeof Q);
point_encode(A, &P);
r &= ~(s & t);
return r;
}


const br_ec_impl br_ec_p256_m62 = {
(uint32_t)0x00800000,
&api_generator,
&api_order,
&api_xoff,
&api_mul,
&api_mulgen,
&api_muladd
};


const br_ec_impl *
br_ec_p256_m62_get(void)
{
return &br_ec_p256_m62;
}

#else


const br_ec_impl *
br_ec_p256_m62_get(void)
{
return 0;
}

#endif
