























#include "inner.h"


void
br_tls12_sha384_prf(void *dst, size_t len,
const void *secret, size_t secret_len, const char *label,
size_t seed_num, const br_tls_prf_seed_chunk *seed)
{
memset(dst, 0, len);
br_tls_phash(dst, len, &br_sha384_vtable,
secret, secret_len, label, seed_num, seed);
}
