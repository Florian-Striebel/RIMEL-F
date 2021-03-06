























#include "inner.h"











static const uint16_t P1305[] = {
0x008A,
0x7FFB, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x03FF
};




#define P0I 0x4CCD





static const uint16_t R2[] = {
0x008A,
0x6400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};





static void
poly1305_inner(uint16_t *a, const uint16_t *r, const void *data, size_t len)
{
const unsigned char *buf;

buf = data;
while (len > 0) {
unsigned char tmp[16], rev[16];
uint16_t b[10];
uint32_t ctl;
int i;




if (len < 16) {
memset(tmp, 0, sizeof tmp);
memcpy(tmp, buf, len);
buf = tmp;
len = 16;
}





for (i = 0; i < 16; i ++) {
rev[i] = buf[15 - i];
}
br_i15_decode_mod(b, rev, sizeof rev, P1305);
b[9] |= 0x0100;





ctl = br_i15_add(b, a, 1);
ctl |= NOT(br_i15_sub(b, P1305, 0));
br_i15_sub(b, P1305, ctl);




br_i15_montymul(a, b, r, P1305, P0I);

buf += 16;
len -= 16;
}
}




static void
byteswap16(unsigned char *buf)
{
int i;

for (i = 0; i < 8; i ++) {
unsigned x;

x = buf[i];
buf[i] = buf[15 - i];
buf[15 - i] = x;
}
}


void
br_poly1305_i15_run(const void *key, const void *iv,
void *data, size_t len, const void *aad, size_t aad_len,
void *tag, br_chacha20_run ichacha, int encrypt)
{
unsigned char pkey[32], foot[16];
uint16_t t[10], r[10], acc[10];





memset(pkey, 0, sizeof pkey);
ichacha(key, iv, 0, pkey, sizeof pkey);





if (encrypt) {
ichacha(key, iv, 1, data, len);
}











pkey[ 3] &= 0x0F;
pkey[ 7] &= 0x0F;
pkey[11] &= 0x0F;
pkey[15] &= 0x0F;
pkey[ 4] &= 0xFC;
pkey[ 8] &= 0xFC;
pkey[12] &= 0xFC;





byteswap16(pkey);
br_i15_decode_mod(t, pkey, 16, P1305);




br_i15_montymul(r, t, R2, P1305, P0I);




br_i15_zero(acc, 0x8A);





br_enc64le(foot, (uint64_t)aad_len);
br_enc64le(foot + 8, (uint64_t)len);
poly1305_inner(acc, r, aad, aad_len);
poly1305_inner(acc, r, data, len);
poly1305_inner(acc, r, foot, sizeof foot);




byteswap16(pkey + 16);
br_i15_decode_mod(t, pkey + 16, 16, P1305);





br_i15_add(acc, t, 1);





br_i15_encode(tag, 16, acc);
byteswap16(tag);




if (!encrypt) {
ichacha(key, iv, 1, data, len);
}
}
