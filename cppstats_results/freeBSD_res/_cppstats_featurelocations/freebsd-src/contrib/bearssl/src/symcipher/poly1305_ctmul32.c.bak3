























#include "inner.h"




static void
poly1305_inner(uint32_t *a, const uint32_t *r, const void *data, size_t len)
{















const unsigned char *buf;

buf = data;
while (len > 0) {
unsigned char tmp[16];
uint32_t b[10];
unsigned u, v;
uint32_t z, cc1, cc2;




if (len < 16) {
memset(tmp, 0, sizeof tmp);
memcpy(tmp, buf, len);
buf = tmp;
len = 16;
}





v = br_dec16le(buf);
a[0] += v & 0x01FFF;
v >>= 13;
v |= buf[2] << 3;
v |= buf[3] << 11;
a[1] += v & 0x01FFF;
v >>= 13;
v |= buf[4] << 6;
a[2] += v & 0x01FFF;
v >>= 13;
v |= buf[5] << 1;
v |= buf[6] << 9;
a[3] += v & 0x01FFF;
v >>= 13;
v |= buf[7] << 4;
v |= buf[8] << 12;
a[4] += v & 0x01FFF;
v >>= 13;
v |= buf[9] << 7;
a[5] += v & 0x01FFF;
v >>= 13;
v |= buf[10] << 2;
v |= buf[11] << 10;
a[6] += v & 0x01FFF;
v >>= 13;
v |= buf[12] << 5;
a[7] += v & 0x01FFF;
v = br_dec16le(buf + 13);
a[8] += v & 0x01FFF;
v >>= 13;
v |= buf[15] << 3;
a[9] += v | 0x00800;



















cc1 = 0;
for (u = 0; u < 10; u ++) {
uint32_t s;

s = cc1
+ MUL15(a[0], r[u + 9 - 0])
+ MUL15(a[1], r[u + 9 - 1])
+ MUL15(a[2], r[u + 9 - 2])
+ MUL15(a[3], r[u + 9 - 3])
+ MUL15(a[4], r[u + 9 - 4]);
b[u] = s & 0x1FFF;
cc1 = s >> 13;
}
cc2 = 0;
for (u = 0; u < 10; u ++) {
uint32_t s;

s = b[u] + cc2
+ MUL15(a[5], r[u + 9 - 5])
+ MUL15(a[6], r[u + 9 - 6])
+ MUL15(a[7], r[u + 9 - 7])
+ MUL15(a[8], r[u + 9 - 8])
+ MUL15(a[9], r[u + 9 - 9]);
b[u] = s & 0x1FFF;
cc2 = s >> 13;
}
memcpy(a, b, sizeof b);





z = cc1 + cc2;
z += (z << 2) + a[0];
a[0] = z & 0x1FFF;
a[1] += z >> 13;

buf += 16;
len -= 16;
}
}


void
br_poly1305_ctmul32_run(const void *key, const void *iv,
void *data, size_t len, const void *aad, size_t aad_len,
void *tag, br_chacha20_run ichacha, int encrypt)
{
unsigned char pkey[32], foot[16];
uint32_t z, r[19], acc[10], cc, ctl;
int i;





memset(pkey, 0, sizeof pkey);
ichacha(key, iv, 0, pkey, sizeof pkey);





if (encrypt) {
ichacha(key, iv, 1, data, len);
}












z = br_dec32le(pkey) & 0x03FFFFFF;
r[9] = z & 0x1FFF;
r[10] = z >> 13;
z = (br_dec32le(pkey + 3) >> 2) & 0x03FFFF03;
r[11] = z & 0x1FFF;
r[12] = z >> 13;
z = (br_dec32le(pkey + 6) >> 4) & 0x03FFC0FF;
r[13] = z & 0x1FFF;
r[14] = z >> 13;
z = (br_dec32le(pkey + 9) >> 6) & 0x03F03FFF;
r[15] = z & 0x1FFF;
r[16] = z >> 13;
z = (br_dec32le(pkey + 12) >> 8) & 0x000FFFFF;
r[17] = z & 0x1FFF;
r[18] = z >> 13;




for (i = 0; i < 9; i ++) {
r[i] = MUL15(5, r[i + 10]);
}




memset(acc, 0, sizeof acc);





br_enc64le(foot, (uint64_t)aad_len);
br_enc64le(foot + 8, (uint64_t)len);
poly1305_inner(acc, r, aad, aad_len);
poly1305_inner(acc, r, data, len);
poly1305_inner(acc, r, foot, sizeof foot);








cc = 0;
for (i = 1; i < 10; i ++) {
z = acc[i] + cc;
acc[i] = z & 0x1FFF;
cc = z >> 13;
}
z = acc[0] + cc + (cc << 2);
acc[0] = z & 0x1FFF;
acc[1] += z >> 13;






ctl = GT(acc[0], 0x1FFA);
for (i = 1; i < 10; i ++) {
ctl &= EQ(acc[i], 0x1FFF);
}
acc[0] = MUX(ctl, acc[0] - 0x1FFB, acc[0]);
for (i = 1; i < 10; i ++) {
acc[i] &= ~(-ctl);
}






z = acc[0] + (acc[1] << 13) + br_dec16le(pkey + 16);
br_enc16le((unsigned char *)tag, z & 0xFFFF);
z = (z >> 16) + (acc[2] << 10) + br_dec16le(pkey + 18);
br_enc16le((unsigned char *)tag + 2, z & 0xFFFF);
z = (z >> 16) + (acc[3] << 7) + br_dec16le(pkey + 20);
br_enc16le((unsigned char *)tag + 4, z & 0xFFFF);
z = (z >> 16) + (acc[4] << 4) + br_dec16le(pkey + 22);
br_enc16le((unsigned char *)tag + 6, z & 0xFFFF);
z = (z >> 16) + (acc[5] << 1) + (acc[6] << 14) + br_dec16le(pkey + 24);
br_enc16le((unsigned char *)tag + 8, z & 0xFFFF);
z = (z >> 16) + (acc[7] << 11) + br_dec16le(pkey + 26);
br_enc16le((unsigned char *)tag + 10, z & 0xFFFF);
z = (z >> 16) + (acc[8] << 8) + br_dec16le(pkey + 28);
br_enc16le((unsigned char *)tag + 12, z & 0xFFFF);
z = (z >> 16) + (acc[9] << 5) + br_dec16le(pkey + 30);
br_enc16le((unsigned char *)tag + 14, z & 0xFFFF);




if (!encrypt) {
ichacha(key, iv, 1, data, len);
}
}
