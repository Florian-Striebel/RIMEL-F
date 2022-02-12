























#include "inner.h"


void
br_tls10_prf(void *dst, size_t len,
const void *secret, size_t secret_len, const char *label,
size_t seed_num, const br_tls_prf_seed_chunk *seed)
{
const unsigned char *s1;
size_t slen;

s1 = secret;
slen = (secret_len + 1) >> 1;
memset(dst, 0, len);
br_tls_phash(dst, len, &br_md5_vtable,
s1, slen, label, seed_num, seed);
br_tls_phash(dst, len, &br_sha1_vtable,
s1 + secret_len - slen, slen, label, seed_num, seed);
}
