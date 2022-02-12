























#include "inner.h"

#if BR_INT128 || BR_UMUL128

#if BR_INT128

#define MUL128(hi, lo, x, y) do { unsigned __int128 mul128tmp; mul128tmp = (unsigned __int128)(x) * (unsigned __int128)(y); (hi) = (uint64_t)(mul128tmp >> 64); (lo) = (uint64_t)mul128tmp; } while (0)






#elif BR_UMUL128

#include <intrin.h>

#define MUL128(hi, lo, x, y) do { (lo) = _umul128((x), (y), &(hi)); } while (0)



#endif

#define MASK42 ((uint64_t)0x000003FFFFFFFFFF)
#define MASK44 ((uint64_t)0x00000FFFFFFFFFFF)

























































static void
poly1305_inner_big(uint64_t *acc, uint64_t *r, const void *data, size_t len)
{

#define MX(hi, lo, m0, m1, m2) do { uint64_t mxhi, mxlo; MUL128(mxhi, mxlo, a0, m0); (hi) = mxhi; (lo) = mxlo >> 20; MUL128(mxhi, mxlo, a1, m1); (hi) += mxhi; (lo) += mxlo >> 20; MUL128(mxhi, mxlo, a2, m2); (hi) += mxhi; (lo) += mxlo >> 20; } while (0)












const unsigned char *buf;
uint64_t a0, a1, a2;
uint64_t r0, r1, r2, t1, t2, u2;

r0 = r[0];
r1 = r[1];
r2 = r[2];
t1 = r[3];
t2 = r[4];
u2 = r[5];
a0 = acc[0];
a1 = acc[1];
a2 = acc[2];
buf = data;

while (len > 0) {
uint64_t v0, v1, v2;
uint64_t c0, c1, c2, d0, d1, d2;

v0 = br_dec64le(buf + 0);
v1 = br_dec64le(buf + 8);
v2 = v1 >> 24;
v1 = ((v0 >> 44) | (v1 << 20)) & MASK44;
v0 &= MASK44;
a0 += v0;
a1 += v1;
a2 += v2 + ((uint64_t)1 << 40);
MX(d0, c0, r0, u2, t1);
MX(d1, c1, r1, r0, t2);
MX(d2, c2, r2, r1, r0);
a0 = c0 + 20 * d2;
a1 = c1 + d0;
a2 = c2 + d1;

v0 = br_dec64le(buf + 16);
v1 = br_dec64le(buf + 24);
v2 = v1 >> 24;
v1 = ((v0 >> 44) | (v1 << 20)) & MASK44;
v0 &= MASK44;
a0 += v0;
a1 += v1;
a2 += v2 + ((uint64_t)1 << 40);
MX(d0, c0, r0, u2, t1);
MX(d1, c1, r1, r0, t2);
MX(d2, c2, r2, r1, r0);
a0 = c0 + 20 * d2;
a1 = c1 + d0;
a2 = c2 + d1;

v0 = br_dec64le(buf + 32);
v1 = br_dec64le(buf + 40);
v2 = v1 >> 24;
v1 = ((v0 >> 44) | (v1 << 20)) & MASK44;
v0 &= MASK44;
a0 += v0;
a1 += v1;
a2 += v2 + ((uint64_t)1 << 40);
MX(d0, c0, r0, u2, t1);
MX(d1, c1, r1, r0, t2);
MX(d2, c2, r2, r1, r0);
a0 = c0 + 20 * d2;
a1 = c1 + d0;
a2 = c2 + d1;

v0 = br_dec64le(buf + 48);
v1 = br_dec64le(buf + 56);
v2 = v1 >> 24;
v1 = ((v0 >> 44) | (v1 << 20)) & MASK44;
v0 &= MASK44;
a0 += v0;
a1 += v1;
a2 += v2 + ((uint64_t)1 << 40);
MX(d0, c0, r0, u2, t1);
MX(d1, c1, r1, r0, t2);
MX(d2, c2, r2, r1, r0);
a0 = c0 + 20 * d2;
a1 = c1 + d0;
a2 = c2 + d1;

a1 += a0 >> 44;
a0 &= MASK44;
a2 += a1 >> 44;
a1 &= MASK44;
a0 += 20 * (a2 >> 44);
a2 &= MASK44;

buf += 64;
len -= 64;
}
acc[0] = a0;
acc[1] = a1;
acc[2] = a2;

#undef MX
}

static void
poly1305_inner_small(uint64_t *acc, uint64_t *r, const void *data, size_t len)
{
const unsigned char *buf;
uint64_t a0, a1, a2;
uint64_t r0, r1, r2, t1, t2, u2;

r0 = r[0];
r1 = r[1];
r2 = r[2];
t1 = r[3];
t2 = r[4];
u2 = r[5];
a0 = acc[0];
a1 = acc[1];
a2 = acc[2];
buf = data;

while (len > 0) {
uint64_t v0, v1, v2;
uint64_t c0, c1, c2, d0, d1, d2;
unsigned char tmp[16];

if (len < 16) {
memcpy(tmp, buf, len);
memset(tmp + len, 0, (sizeof tmp) - len);
buf = tmp;
len = 16;
}
v0 = br_dec64le(buf + 0);
v1 = br_dec64le(buf + 8);

v2 = v1 >> 24;
v1 = ((v0 >> 44) | (v1 << 20)) & MASK44;
v0 &= MASK44;

a0 += v0;
a1 += v1;
a2 += v2 + ((uint64_t)1 << 40);

#define MX(hi, lo, m0, m1, m2) do { uint64_t mxhi, mxlo; MUL128(mxhi, mxlo, a0, m0); (hi) = mxhi; (lo) = mxlo >> 20; MUL128(mxhi, mxlo, a1, m1); (hi) += mxhi; (lo) += mxlo >> 20; MUL128(mxhi, mxlo, a2, m2); (hi) += mxhi; (lo) += mxlo >> 20; } while (0)












MX(d0, c0, r0, u2, t1);
MX(d1, c1, r1, r0, t2);
MX(d2, c2, r2, r1, r0);

#undef MX

a0 = c0 + 20 * d2;
a1 = c1 + d0;
a2 = c2 + d1;

a1 += a0 >> 44;
a0 &= MASK44;
a2 += a1 >> 44;
a1 &= MASK44;
a0 += 20 * (a2 >> 44);
a2 &= MASK44;

buf += 16;
len -= 16;
}
acc[0] = a0;
acc[1] = a1;
acc[2] = a2;
}

static inline void
poly1305_inner(uint64_t *acc, uint64_t *r, const void *data, size_t len)
{
if (len >= 64) {
size_t len2;

len2 = len & ~(size_t)63;
poly1305_inner_big(acc, r, data, len2);
data = (const unsigned char *)data + len2;
len -= len2;
}
if (len > 0) {
poly1305_inner_small(acc, r, data, len);
}
}


void
br_poly1305_ctmulq_run(const void *key, const void *iv,
void *data, size_t len, const void *aad, size_t aad_len,
void *tag, br_chacha20_run ichacha, int encrypt)
{
unsigned char pkey[32], foot[16];
uint64_t r[6], acc[3], r0, r1;
uint32_t v0, v1, v2, v3, v4;
uint64_t w0, w1, w2, w3;
uint32_t ctl;





memset(pkey, 0, sizeof pkey);
ichacha(key, iv, 0, pkey, sizeof pkey);





if (encrypt) {
ichacha(key, iv, 1, data, len);
}











pkey[ 3] &= 0x0F;
pkey[ 4] &= 0xFC;
pkey[ 7] &= 0x0F;
pkey[ 8] &= 0xFC;
pkey[11] &= 0x0F;
pkey[12] &= 0xFC;
pkey[15] &= 0x0F;





r0 = br_dec64le(pkey + 0);
r1 = br_dec64le(pkey + 8);
r[0] = r0 << 20;
r[1] = ((r0 >> 24) | (r1 << 40)) & ~(uint64_t)0xFFFFF;
r[2] = (r1 >> 4) & ~(uint64_t)0xFFFFF;
r1 = 20 * (r[1] >> 20);
r[3] = r1 << 20;
r[5] = 20 * r[2];
r[4] = (r[5] + (r1 >> 24)) & ~(uint64_t)0xFFFFF;




acc[0] = 0;
acc[1] = 0;
acc[2] = 0;





br_enc64le(foot, (uint64_t)aad_len);
br_enc64le(foot + 8, (uint64_t)len);
poly1305_inner(acc, r, aad, aad_len);
poly1305_inner(acc, r, data, len);
poly1305_inner_small(acc, r, foot, sizeof foot);






acc[1] += (acc[0] >> 44);
acc[0] &= MASK44;
acc[2] += (acc[1] >> 44);
acc[1] &= MASK44;
acc[0] += 5 * (acc[2] >> 42);
acc[2] &= MASK42;
acc[1] += (acc[0] >> 44);
acc[0] &= MASK44;
acc[2] += (acc[1] >> 44);
acc[1] &= MASK44;
acc[0] += 5 * (acc[2] >> 42);
acc[2] &= MASK42;







v0 = (uint32_t)acc[0];
v1 = (uint32_t)(acc[0] >> 32) | ((uint32_t)acc[1] << 12);
v2 = (uint32_t)(acc[1] >> 20) | ((uint32_t)acc[2] << 24);
v3 = (uint32_t)(acc[2] >> 8);
v4 = (uint32_t)(acc[2] >> 40);

ctl = GT(v0, 0xFFFFFFFA);
ctl &= EQ(v1, 0xFFFFFFFF);
ctl &= EQ(v2, 0xFFFFFFFF);
ctl &= EQ(v3, 0xFFFFFFFF);
ctl &= EQ(v4, 0x00000003);
v0 = MUX(ctl, v0 + 5, v0);
v1 = MUX(ctl, 0, v1);
v2 = MUX(ctl, 0, v2);
v3 = MUX(ctl, 0, v3);





w0 = (uint64_t)v0 + (uint64_t)br_dec32le(pkey + 16);
w1 = (uint64_t)v1 + (uint64_t)br_dec32le(pkey + 20) + (w0 >> 32);
w2 = (uint64_t)v2 + (uint64_t)br_dec32le(pkey + 24) + (w1 >> 32);
w3 = (uint64_t)v3 + (uint64_t)br_dec32le(pkey + 28) + (w2 >> 32);
v0 = (uint32_t)w0;
v1 = (uint32_t)w1;
v2 = (uint32_t)w2;
v3 = (uint32_t)w3;




br_enc32le((unsigned char *)tag + 0, v0);
br_enc32le((unsigned char *)tag + 4, v1);
br_enc32le((unsigned char *)tag + 8, v2);
br_enc32le((unsigned char *)tag + 12, v3);




if (!encrypt) {
ichacha(key, iv, 1, data, len);
}
}


br_poly1305_run
br_poly1305_ctmulq_get(void)
{
return &br_poly1305_ctmulq_run;
}

#else


br_poly1305_run
br_poly1305_ctmulq_get(void)
{
return 0;
}

#endif
