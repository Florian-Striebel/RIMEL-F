























#include "inner.h"






static void
hash_data(const br_hash_class *dig, void *dst, const void *src, size_t len)
{
br_hash_compat_context hc;

hc.vtable = dig;
dig->init(&hc.vtable);
dig->update(&hc.vtable, src, len);
dig->out(&hc.vtable, dst);
}


size_t
br_rsa_oaep_pad(const br_prng_class **rnd, const br_hash_class *dig,
const void *label, size_t label_len,
const br_rsa_public_key *pk,
void *dst, size_t dst_max_len,
const void *src, size_t src_len)
{
size_t k, hlen;
unsigned char *buf;

hlen = br_digest_size(dig);




k = pk->nlen;
while (k > 0 && pk->n[k - 1] == 0) {
k --;
}







if (k < ((hlen << 1) + 2)
|| src_len > (k - (hlen << 1) - 2)
|| dst_max_len < k)
{
return 0;
}




buf = dst;






memmove(buf + k - src_len, src, src_len);
hash_data(dig, buf + 1 + hlen, label, label_len);
memset(buf + 1 + (hlen << 1), 0, k - src_len - (hlen << 1) - 2);
buf[k - src_len - 1] = 0x01;




(*rnd)->generate(rnd, buf + 1, hlen);




br_mgf1_xor(buf + 1 + hlen, k - hlen - 1, dig, buf + 1, hlen);




br_mgf1_xor(buf + 1, hlen, dig, buf + 1 + hlen, k - hlen - 1);




buf[0] = 0x00;
return k;
}
