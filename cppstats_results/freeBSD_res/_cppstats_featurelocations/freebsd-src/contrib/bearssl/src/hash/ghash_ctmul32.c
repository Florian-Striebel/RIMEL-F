























#include "inner.h"






















static inline uint32_t
bmul32(uint32_t x, uint32_t y)
{
uint32_t x0, x1, x2, x3;
uint32_t y0, y1, y2, y3;
uint32_t z0, z1, z2, z3;

x0 = x & (uint32_t)0x11111111;
x1 = x & (uint32_t)0x22222222;
x2 = x & (uint32_t)0x44444444;
x3 = x & (uint32_t)0x88888888;
y0 = y & (uint32_t)0x11111111;
y1 = y & (uint32_t)0x22222222;
y2 = y & (uint32_t)0x44444444;
y3 = y & (uint32_t)0x88888888;
z0 = (x0 * y0) ^ (x1 * y3) ^ (x2 * y2) ^ (x3 * y1);
z1 = (x0 * y1) ^ (x1 * y0) ^ (x2 * y3) ^ (x3 * y2);
z2 = (x0 * y2) ^ (x1 * y1) ^ (x2 * y0) ^ (x3 * y3);
z3 = (x0 * y3) ^ (x1 * y2) ^ (x2 * y1) ^ (x3 * y0);
z0 &= (uint32_t)0x11111111;
z1 &= (uint32_t)0x22222222;
z2 &= (uint32_t)0x44444444;
z3 &= (uint32_t)0x88888888;
return z0 | z1 | z2 | z3;
}




static uint32_t
rev32(uint32_t x)
{
#define RMS(m, s) do { x = ((x & (uint32_t)(m)) << (s)) | ((x >> (s)) & (uint32_t)(m)); } while (0)




RMS(0x55555555, 1);
RMS(0x33333333, 2);
RMS(0x0F0F0F0F, 4);
RMS(0x00FF00FF, 8);
return (x << 16) | (x >> 16);

#undef RMS
}


void
br_ghash_ctmul32(void *y, const void *h, const void *data, size_t len)
{







const unsigned char *buf, *hb;
unsigned char *yb;
uint32_t yw[4];
uint32_t hw[4], hwr[4];

buf = data;
yb = y;
hb = h;
yw[3] = br_dec32be(yb);
yw[2] = br_dec32be(yb + 4);
yw[1] = br_dec32be(yb + 8);
yw[0] = br_dec32be(yb + 12);
hw[3] = br_dec32be(hb);
hw[2] = br_dec32be(hb + 4);
hw[1] = br_dec32be(hb + 8);
hw[0] = br_dec32be(hb + 12);
hwr[3] = rev32(hw[3]);
hwr[2] = rev32(hw[2]);
hwr[1] = rev32(hw[1]);
hwr[0] = rev32(hw[0]);
while (len > 0) {
const unsigned char *src;
unsigned char tmp[16];
int i;
uint32_t a[18], b[18], c[18];
uint32_t d0, d1, d2, d3, d4, d5, d6, d7;
uint32_t zw[8];

if (len >= 16) {
src = buf;
buf += 16;
len -= 16;
} else {
memcpy(tmp, buf, len);
memset(tmp + len, 0, (sizeof tmp) - len);
src = tmp;
len = 0;
}
yw[3] ^= br_dec32be(src);
yw[2] ^= br_dec32be(src + 4);
yw[1] ^= br_dec32be(src + 8);
yw[0] ^= br_dec32be(src + 12);














a[0] = yw[0];
a[1] = yw[1];
a[2] = yw[2];
a[3] = yw[3];
a[4] = a[0] ^ a[1];
a[5] = a[2] ^ a[3];
a[6] = a[0] ^ a[2];
a[7] = a[1] ^ a[3];
a[8] = a[6] ^ a[7];

a[ 9] = rev32(yw[0]);
a[10] = rev32(yw[1]);
a[11] = rev32(yw[2]);
a[12] = rev32(yw[3]);
a[13] = a[ 9] ^ a[10];
a[14] = a[11] ^ a[12];
a[15] = a[ 9] ^ a[11];
a[16] = a[10] ^ a[12];
a[17] = a[15] ^ a[16];

b[0] = hw[0];
b[1] = hw[1];
b[2] = hw[2];
b[3] = hw[3];
b[4] = b[0] ^ b[1];
b[5] = b[2] ^ b[3];
b[6] = b[0] ^ b[2];
b[7] = b[1] ^ b[3];
b[8] = b[6] ^ b[7];

b[ 9] = hwr[0];
b[10] = hwr[1];
b[11] = hwr[2];
b[12] = hwr[3];
b[13] = b[ 9] ^ b[10];
b[14] = b[11] ^ b[12];
b[15] = b[ 9] ^ b[11];
b[16] = b[10] ^ b[12];
b[17] = b[15] ^ b[16];

for (i = 0; i < 18; i ++) {
c[i] = bmul32(a[i], b[i]);
}

c[4] ^= c[0] ^ c[1];
c[5] ^= c[2] ^ c[3];
c[8] ^= c[6] ^ c[7];

c[13] ^= c[ 9] ^ c[10];
c[14] ^= c[11] ^ c[12];
c[17] ^= c[15] ^ c[16];






d0 = c[0];
d1 = c[4] ^ (rev32(c[9]) >> 1);
d2 = c[1] ^ c[0] ^ c[2] ^ c[6] ^ (rev32(c[13]) >> 1);
d3 = c[4] ^ c[5] ^ c[8]
^ (rev32(c[10] ^ c[9] ^ c[11] ^ c[15]) >> 1);
d4 = c[2] ^ c[1] ^ c[3] ^ c[7]
^ (rev32(c[13] ^ c[14] ^ c[17]) >> 1);
d5 = c[5] ^ (rev32(c[11] ^ c[10] ^ c[12] ^ c[16]) >> 1);
d6 = c[3] ^ (rev32(c[14]) >> 1);
d7 = rev32(c[12]) >> 1;

zw[0] = d0 << 1;
zw[1] = (d1 << 1) | (d0 >> 31);
zw[2] = (d2 << 1) | (d1 >> 31);
zw[3] = (d3 << 1) | (d2 >> 31);
zw[4] = (d4 << 1) | (d3 >> 31);
zw[5] = (d5 << 1) | (d4 >> 31);
zw[6] = (d6 << 1) | (d5 >> 31);
zw[7] = (d7 << 1) | (d6 >> 31);

for (i = 0; i < 4; i ++) {
uint32_t lw;

lw = zw[i];
zw[i + 4] ^= lw ^ (lw >> 1) ^ (lw >> 2) ^ (lw >> 7);
zw[i + 3] ^= (lw << 31) ^ (lw << 30) ^ (lw << 25);
}
memcpy(yw, zw + 4, sizeof yw);
}
br_enc32be(yb, yw[3]);
br_enc32be(yb + 4, yw[2]);
br_enc32be(yb + 8, yw[1]);
br_enc32be(yb + 12, yw[0]);
}
