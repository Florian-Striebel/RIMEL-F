























#include "inner.h"


size_t
br_ec_keygen(const br_prng_class **rng_ctx,
const br_ec_impl *impl, br_ec_private_key *sk,
void *kbuf, int curve)
{
const unsigned char *order;
unsigned char *buf;
size_t len;
unsigned mask;

if (curve < 0 || curve >= 32
|| ((impl->supported_curves >> curve) & 1) == 0)
{
return 0;
}
order = impl->order(curve, &len);
while (len > 0 && *order == 0) {
order ++;
len --;
}
if (kbuf == NULL || len == 0) {
return len;
}
mask = order[0];
mask |= (mask >> 1);
mask |= (mask >> 2);
mask |= (mask >> 4);






buf = kbuf;
for (;;) {
size_t u;
unsigned cc, zz;

(*rng_ctx)->generate(rng_ctx, buf, len);
buf[0] &= mask;
cc = 0;
u = len;
zz = 0;
while (u -- > 0) {
cc = ((unsigned)(buf[u] - order[u] - cc) >> 8) & 1;
zz |= buf[u];
}
if (cc != 0 && zz != 0) {
break;
}
}

if (sk != NULL) {
sk->curve = curve;
sk->x = buf;
sk->xlen = len;
}
return len;
}
