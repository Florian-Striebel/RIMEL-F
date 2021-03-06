























#include "inner.h"













static const unsigned char QL0[] = {
17, 4, 27, 23, 13, 22, 7, 18,
16, 24, 2, 20, 1, 8, 15, 26
};

static const unsigned char QR0[] = {
25, 19, 9, 1, 5, 11, 23, 8,
17, 0, 22, 3, 6, 20, 27, 24
};

static const unsigned char QL1[] = {
28, 28, 14, 11, 28, 28, 25, 0,
28, 28, 5, 9, 28, 28, 12, 21
};

static const unsigned char QR1[] = {
28, 28, 15, 4, 28, 28, 26, 16,
28, 28, 12, 7, 28, 28, 10, 14
};





static inline uint32_t
rotl(uint32_t x, int n)
{
return (x << n) | (x >> (32 - n));
}




static void
keysched_unit(uint32_t *skey, const void *key)
{
int i;

br_des_keysched_unit(skey, key);




for (i = 0; i < 16; i ++) {
uint32_t kl, kr, sk0, sk1;
int j;

kl = skey[(i << 1) + 0];
kr = skey[(i << 1) + 1];
sk0 = 0;
sk1 = 0;
for (j = 0; j < 16; j ++) {
sk0 <<= 1;
sk1 <<= 1;
sk0 |= ((kl >> QL0[j]) & (uint32_t)1) << 16;
sk0 |= (kr >> QR0[j]) & (uint32_t)1;
sk1 |= ((kl >> QL1[j]) & (uint32_t)1) << 16;
sk1 |= (kr >> QR1[j]) & (uint32_t)1;
}

skey[(i << 1) + 0] = sk0;
skey[(i << 1) + 1] = sk1;
}

#if 0




sk0 = kl & (uint32_t)0x00100000;
sk0 |= (kl & (uint32_t)0x08008000) << 2;
sk0 |= (kl & (uint32_t)0x00400000) << 4;
sk0 |= (kl & (uint32_t)0x00800000) << 5;
sk0 |= (kl & (uint32_t)0x00040000) << 6;
sk0 |= (kl & (uint32_t)0x00010000) << 7;
sk0 |= (kl & (uint32_t)0x00000100) << 10;
sk0 |= (kl & (uint32_t)0x00022000) << 14;
sk0 |= (kl & (uint32_t)0x00000082) << 18;
sk0 |= (kl & (uint32_t)0x00000004) << 19;
sk0 |= (kl & (uint32_t)0x04000000) >> 10;
sk0 |= (kl & (uint32_t)0x00000010) << 26;
sk0 |= (kl & (uint32_t)0x01000000) >> 2;

sk0 |= kr & (uint32_t)0x00000100;
sk0 |= (kr & (uint32_t)0x00000008) << 1;
sk0 |= (kr & (uint32_t)0x00000200) << 4;
sk0 |= rotl(kr & (uint32_t)0x08000021, 6);
sk0 |= (kr & (uint32_t)0x01000000) >> 24;
sk0 |= (kr & (uint32_t)0x00000002) << 11;
sk0 |= (kr & (uint32_t)0x00100000) >> 18;
sk0 |= (kr & (uint32_t)0x00400000) >> 17;
sk0 |= (kr & (uint32_t)0x00800000) >> 14;
sk0 |= (kr & (uint32_t)0x02020000) >> 10;
sk0 |= (kr & (uint32_t)0x00080000) >> 5;
sk0 |= (kr & (uint32_t)0x00000040) >> 3;
sk0 |= (kr & (uint32_t)0x00000800) >> 1;

sk1 = kl & (uint32_t)0x02000000;
sk1 |= (kl & (uint32_t)0x00001000) << 5;
sk1 |= (kl & (uint32_t)0x00000200) << 11;
sk1 |= (kl & (uint32_t)0x00004000) << 15;
sk1 |= (kl & (uint32_t)0x00000020) << 16;
sk1 |= (kl & (uint32_t)0x00000800) << 17;
sk1 |= (kl & (uint32_t)0x00000001) << 24;
sk1 |= (kl & (uint32_t)0x00200000) >> 5;

sk1 |= (kr & (uint32_t)0x00000010) << 8;
sk1 |= (kr & (uint32_t)0x04000000) >> 17;
sk1 |= (kr & (uint32_t)0x00004000) >> 14;
sk1 |= (kr & (uint32_t)0x00000400) >> 9;
sk1 |= (kr & (uint32_t)0x00010000) >> 8;
sk1 |= (kr & (uint32_t)0x00001000) >> 7;
sk1 |= (kr & (uint32_t)0x00000080) >> 3;
sk1 |= (kr & (uint32_t)0x00008000) >> 2;
#endif
}


unsigned
br_des_ct_keysched(uint32_t *skey, const void *key, size_t key_len)
{
switch (key_len) {
case 8:
keysched_unit(skey, key);
return 1;
case 16:
keysched_unit(skey, key);
keysched_unit(skey + 32, (const unsigned char *)key + 8);
br_des_rev_skey(skey + 32);
memcpy(skey + 64, skey, 32 * sizeof *skey);
return 3;
default:
keysched_unit(skey, key);
keysched_unit(skey + 32, (const unsigned char *)key + 8);
br_des_rev_skey(skey + 32);
keysched_unit(skey + 64, (const unsigned char *)key + 16);
return 3;
}
}





static inline uint32_t
Fconf(uint32_t r0, const uint32_t *sk)
{



















uint32_t x0, x1, x2, x3, x4, x5, z0;
uint32_t y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
uint32_t y10, y11, y12, y13, y14, y15, y16, y17, y18, y19;
uint32_t y20, y21, y22, y23, y24, y25, y26, y27, y28, y29;
uint32_t y30;




x1 = r0 & (uint32_t)0x11111111;
x2 = (r0 >> 1) & (uint32_t)0x11111111;
x3 = (r0 >> 2) & (uint32_t)0x11111111;
x4 = (r0 >> 3) & (uint32_t)0x11111111;
x1 = (x1 << 4) - x1;
x2 = (x2 << 4) - x2;
x3 = (x3 << 4) - x3;
x4 = (x4 << 4) - x4;
x0 = (x4 << 4) | (x4 >> 28);
x5 = (x1 >> 4) | (x1 << 28);




x0 ^= sk[0];
x1 ^= sk[1];
x2 ^= sk[2];
x3 ^= sk[3];
x4 ^= sk[4];
x5 ^= sk[5];









y0 = (uint32_t)0xEFA72C4D ^ (x0 & (uint32_t)0xEC7AC69C);
y1 = (uint32_t)0xAEAAEDFF ^ (x0 & (uint32_t)0x500FB821);
y2 = (uint32_t)0x37396665 ^ (x0 & (uint32_t)0x40EFA809);
y3 = (uint32_t)0x68D7B833 ^ (x0 & (uint32_t)0xA5EC0B28);
y4 = (uint32_t)0xC9C755BB ^ (x0 & (uint32_t)0x252CF820);
y5 = (uint32_t)0x73FC3606 ^ (x0 & (uint32_t)0x40205801);
y6 = (uint32_t)0xA2A0A918 ^ (x0 & (uint32_t)0xE220F929);
y7 = (uint32_t)0x8222BD90 ^ (x0 & (uint32_t)0x44A3F9E1);
y8 = (uint32_t)0xD6B6AC77 ^ (x0 & (uint32_t)0x794F104A);
y9 = (uint32_t)0x3069300C ^ (x0 & (uint32_t)0x026F320B);
y10 = (uint32_t)0x6CE0D5CC ^ (x0 & (uint32_t)0x7640B01A);
y11 = (uint32_t)0x59A9A22D ^ (x0 & (uint32_t)0x238F1572);
y12 = (uint32_t)0xAC6D0BD4 ^ (x0 & (uint32_t)0x7A63C083);
y13 = (uint32_t)0x21C83200 ^ (x0 & (uint32_t)0x11CCA000);
y14 = (uint32_t)0xA0E62188 ^ (x0 & (uint32_t)0x202F69AA);

y16 = (uint32_t)0xAF7D655A ^ (x0 & (uint32_t)0x51B33BE9);
y17 = (uint32_t)0xF0168AA3 ^ (x0 & (uint32_t)0x3B0FE8AE);
y18 = (uint32_t)0x90AA30C6 ^ (x0 & (uint32_t)0x90BF8816);
y19 = (uint32_t)0x5AB2750A ^ (x0 & (uint32_t)0x09E34F9B);
y20 = (uint32_t)0x5391BE65 ^ (x0 & (uint32_t)0x0103BE88);
y21 = (uint32_t)0x93372BAF ^ (x0 & (uint32_t)0x49AC8E25);
y22 = (uint32_t)0xF288210C ^ (x0 & (uint32_t)0x922C313D);
y23 = (uint32_t)0x920AF5C0 ^ (x0 & (uint32_t)0x70EF31B0);
y24 = (uint32_t)0x63D312C0 ^ (x0 & (uint32_t)0x6A707100);
y25 = (uint32_t)0x537B3006 ^ (x0 & (uint32_t)0xB97C9011);
y26 = (uint32_t)0xA2EFB0A5 ^ (x0 & (uint32_t)0xA320C959);
y27 = (uint32_t)0xBC8F96A5 ^ (x0 & (uint32_t)0x6EA0AB4A);
y28 = (uint32_t)0xFAD176A5 ^ (x0 & (uint32_t)0x6953DDF8);
y29 = (uint32_t)0x665A14A3 ^ (x0 & (uint32_t)0xF74F3E2B);
y30 = (uint32_t)0xF2EFF0CC ^ (x0 & (uint32_t)0xF0306CAD);


y0 = y0 ^ (x1 & y1);
y1 = y2 ^ (x1 & y3);
y2 = y4 ^ (x1 & y5);
y3 = y6 ^ (x1 & y7);
y4 = y8 ^ (x1 & y9);
y5 = y10 ^ (x1 & y11);
y6 = y12 ^ (x1 & y13);
y7 = y14;
y8 = y16 ^ (x1 & y17);
y9 = y18 ^ (x1 & y19);
y10 = y20 ^ (x1 & y21);
y11 = y22 ^ (x1 & y23);
y12 = y24 ^ (x1 & y25);
y13 = y26 ^ (x1 & y27);
y14 = y28 ^ (x1 & y29);
y15 = y30;

y0 = y0 ^ (x2 & y1);
y1 = y2 ^ (x2 & y3);
y2 = y4 ^ (x2 & y5);
y3 = y6 ^ (x2 & y7);
y4 = y8 ^ (x2 & y9);
y5 = y10 ^ (x2 & y11);
y6 = y12 ^ (x2 & y13);
y7 = y14 ^ (x2 & y15);

y0 = y0 ^ (x3 & y1);
y1 = y2 ^ (x3 & y3);
y2 = y4 ^ (x3 & y5);
y3 = y6 ^ (x3 & y7);

y0 = y0 ^ (x4 & y1);
y1 = y2 ^ (x4 & y3);

y0 = y0 ^ (x5 & y1);









z0 = (y0 & (uint32_t)0x00000004) << 3;
z0 |= (y0 & (uint32_t)0x00004000) << 4;
z0 |= rotl(y0 & 0x12020120, 5);
z0 |= (y0 & (uint32_t)0x00100000) << 6;
z0 |= (y0 & (uint32_t)0x00008000) << 9;
z0 |= (y0 & (uint32_t)0x04000000) >> 22;
z0 |= (y0 & (uint32_t)0x00000001) << 11;
z0 |= rotl(y0 & 0x20000200, 12);
z0 |= (y0 & (uint32_t)0x00200000) >> 19;
z0 |= (y0 & (uint32_t)0x00000040) << 14;
z0 |= (y0 & (uint32_t)0x00010000) << 15;
z0 |= (y0 & (uint32_t)0x00000002) << 16;
z0 |= rotl(y0 & 0x40801800, 17);
z0 |= (y0 & (uint32_t)0x00080000) >> 13;
z0 |= (y0 & (uint32_t)0x00000010) << 21;
z0 |= (y0 & (uint32_t)0x01000000) >> 10;
z0 |= rotl(y0 & 0x88000008, 24);
z0 |= (y0 & (uint32_t)0x00000480) >> 7;
z0 |= (y0 & (uint32_t)0x00442000) >> 6;
return z0;
}





static void
process_block_unit(uint32_t *pl, uint32_t *pr, const uint32_t *sk_exp)
{
int i;
uint32_t l, r;

l = *pl;
r = *pr;
for (i = 0; i < 16; i ++) {
uint32_t t;

t = l ^ Fconf(r, sk_exp);
l = r;
r = t;
sk_exp += 6;
}
*pl = r;
*pr = l;
}


void
br_des_ct_process_block(unsigned num_rounds,
const uint32_t *sk_exp, void *block)
{
unsigned char *buf;
uint32_t l, r;

buf = block;
l = br_dec32be(buf);
r = br_dec32be(buf + 4);
br_des_do_IP(&l, &r);
while (num_rounds -- > 0) {
process_block_unit(&l, &r, sk_exp);
sk_exp += 96;
}
br_des_do_invIP(&l, &r);
br_enc32be(buf, l);
br_enc32be(buf + 4, r);
}


void
br_des_ct_skey_expand(uint32_t *sk_exp,
unsigned num_rounds, const uint32_t *skey)
{
num_rounds <<= 4;
while (num_rounds -- > 0) {
uint32_t v, w0, w1, w2, w3;

v = *skey ++;
w0 = v & 0x11111111;
w1 = (v >> 1) & 0x11111111;
w2 = (v >> 2) & 0x11111111;
w3 = (v >> 3) & 0x11111111;
*sk_exp ++ = (w0 << 4) - w0;
*sk_exp ++ = (w1 << 4) - w1;
*sk_exp ++ = (w2 << 4) - w2;
*sk_exp ++ = (w3 << 4) - w3;
v = *skey ++;
w0 = v & 0x11111111;
w1 = (v >> 1) & 0x11111111;
*sk_exp ++ = (w0 << 4) - w0;
*sk_exp ++ = (w1 << 4) - w1;
}
}
