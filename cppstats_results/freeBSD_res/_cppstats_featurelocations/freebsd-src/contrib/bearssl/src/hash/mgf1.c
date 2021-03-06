























#include "inner.h"


void
br_mgf1_xor(void *data, size_t len,
const br_hash_class *dig, const void *seed, size_t seed_len)
{
unsigned char *buf;
size_t u, hlen;
uint32_t c;

buf = data;
hlen = br_digest_size(dig);
for (u = 0, c = 0; u < len; u += hlen, c ++) {
br_hash_compat_context hc;
unsigned char tmp[64];
size_t v;

hc.vtable = dig;
dig->init(&hc.vtable);
dig->update(&hc.vtable, seed, seed_len);
br_enc32be(tmp, c);
dig->update(&hc.vtable, tmp, 4);
dig->out(&hc.vtable, tmp);
for (v = 0; v < hlen; v ++) {
if ((u + v) >= len) {
break;
}
buf[u + v] ^= tmp[v];
}
}
}
