























#include "inner.h"


void
br_des_do_IP(uint32_t *xl, uint32_t *xr)
{





uint32_t l, r, t;

l = *xl;
r = *xr;
t = ((l >> 4) ^ r) & (uint32_t)0x0F0F0F0F;
r ^= t;
l ^= t << 4;
t = ((l >> 16) ^ r) & (uint32_t)0x0000FFFF;
r ^= t;
l ^= t << 16;
t = ((r >> 2) ^ l) & (uint32_t)0x33333333;
l ^= t;
r ^= t << 2;
t = ((r >> 8) ^ l) & (uint32_t)0x00FF00FF;
l ^= t;
r ^= t << 8;
t = ((l >> 1) ^ r) & (uint32_t)0x55555555;
r ^= t;
l ^= t << 1;
*xl = l;
*xr = r;
}


void
br_des_do_invIP(uint32_t *xl, uint32_t *xr)
{



uint32_t l, r, t;

l = *xl;
r = *xr;
t = ((l >> 1) ^ r) & 0x55555555;
r ^= t;
l ^= t << 1;
t = ((r >> 8) ^ l) & 0x00FF00FF;
l ^= t;
r ^= t << 8;
t = ((r >> 2) ^ l) & 0x33333333;
l ^= t;
r ^= t << 2;
t = ((l >> 16) ^ r) & 0x0000FFFF;
r ^= t;
l ^= t << 16;
t = ((l >> 4) ^ r) & 0x0F0F0F0F;
r ^= t;
l ^= t << 4;
*xl = l;
*xr = r;
}


void
br_des_keysched_unit(uint32_t *skey, const void *key)
{
uint32_t xl, xr, kl, kr;
int i;

xl = br_dec32be(key);
xr = br_dec32be((const unsigned char *)key + 4);























br_des_do_IP(&xl, &xr);
kl = ((xr & (uint32_t)0xFF000000) >> 4)
| ((xl & (uint32_t)0xFF000000) >> 12)
| ((xr & (uint32_t)0x00FF0000) >> 12)
| ((xl & (uint32_t)0x00FF0000) >> 20);
kr = ((xr & (uint32_t)0x000000FF) << 20)
| ((xl & (uint32_t)0x0000FF00) << 4)
| ((xr & (uint32_t)0x0000FF00) >> 4)
| ((xl & (uint32_t)0x000F0000) >> 16);





for (i = 0; i < 16; i ++) {
if ((1 << i) & 0x8103) {
kl = (kl << 1) | (kl >> 27);
kr = (kr << 1) | (kr >> 27);
} else {
kl = (kl << 2) | (kl >> 26);
kr = (kr << 2) | (kr >> 26);
}
kl &= (uint32_t)0x0FFFFFFF;
kr &= (uint32_t)0x0FFFFFFF;
skey[(i << 1) + 0] = kl;
skey[(i << 1) + 1] = kr;
}
}


void
br_des_rev_skey(uint32_t *skey)
{
int i;

for (i = 0; i < 16; i += 2) {
uint32_t t;

t = skey[i + 0];
skey[i + 0] = skey[30 - i];
skey[30 - i] = t;
t = skey[i + 1];
skey[i + 1] = skey[31 - i];
skey[31 - i] = t;
}
}
