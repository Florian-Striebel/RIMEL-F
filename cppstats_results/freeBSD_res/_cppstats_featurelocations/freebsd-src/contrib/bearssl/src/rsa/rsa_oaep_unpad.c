























#include "inner.h"






static void
xor_hash_data(const br_hash_class *dig, void *dst, const void *src, size_t len)
{
br_hash_compat_context hc;
unsigned char tmp[64];
unsigned char *buf;
size_t u, hlen;

hc.vtable = dig;
dig->init(&hc.vtable);
dig->update(&hc.vtable, src, len);
dig->out(&hc.vtable, tmp);
buf = dst;
hlen = br_digest_size(dig);
for (u = 0; u < hlen; u ++) {
buf[u] ^= tmp[u];
}
}


uint32_t
br_rsa_oaep_unpad(const br_hash_class *dig,
const void *label, size_t label_len,
void *data, size_t *len)
{
size_t u, k, hlen;
unsigned char *buf;
uint32_t r, s, zlen;

hlen = br_digest_size(dig);
k = *len;
buf = data;




if (k < ((hlen << 1) + 2)) {
return 0;
}




br_mgf1_xor(buf + 1, hlen, dig, buf + 1 + hlen, k - hlen - 1);
br_mgf1_xor(buf + 1 + hlen, k - hlen - 1, dig, buf + 1, hlen);





xor_hash_data(dig, buf + 1 + hlen, label, label_len);



























r = 1 - ((buf[0] + 0xFF) >> 8);
s = 0;
zlen = 0;
for (u = hlen + 1; u < k; u ++) {
uint32_t w, nz;

w = buf[u];




nz = r & ((w + 0xFF) >> 8);
s |= nz & EQ(w, 0x01);
r &= NOT(nz);
zlen += r;
}




s &= GE(zlen, (uint32_t)hlen);





if (s) {
size_t plen;

plen = 2 + hlen + zlen;
k -= plen;
memmove(buf, buf + plen, k);
*len = k;
}
return s;
}
