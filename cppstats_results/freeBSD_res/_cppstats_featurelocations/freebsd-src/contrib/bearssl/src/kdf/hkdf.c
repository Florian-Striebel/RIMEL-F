























#include "inner.h"

const unsigned char br_hkdf_no_salt = 0;


void
br_hkdf_init(br_hkdf_context *hc, const br_hash_class *digest_vtable,
const void *salt, size_t salt_len)
{
br_hmac_key_context kc;
unsigned char tmp[64];

if (salt == BR_HKDF_NO_SALT) {
salt = tmp;
salt_len = br_digest_size(digest_vtable);
memset(tmp, 0, salt_len);
}
br_hmac_key_init(&kc, digest_vtable, salt, salt_len);
br_hmac_init(&hc->u.hmac_ctx, &kc, 0);
hc->dig_len = br_hmac_size(&hc->u.hmac_ctx);
}


void
br_hkdf_inject(br_hkdf_context *hc, const void *ikm, size_t ikm_len)
{
br_hmac_update(&hc->u.hmac_ctx, ikm, ikm_len);
}


void
br_hkdf_flip(br_hkdf_context *hc)
{
unsigned char tmp[64];

br_hmac_out(&hc->u.hmac_ctx, tmp);
br_hmac_key_init(&hc->u.prk_ctx,
br_hmac_get_digest(&hc->u.hmac_ctx), tmp, hc->dig_len);
hc->ptr = hc->dig_len;
hc->chunk_num = 0;
}


size_t
br_hkdf_produce(br_hkdf_context *hc,
const void *info, size_t info_len, void *out, size_t out_len)
{
size_t tlen;

tlen = 0;
while (out_len > 0) {
size_t clen;

if (hc->ptr == hc->dig_len) {
br_hmac_context hmac_ctx;
unsigned char x;

hc->chunk_num ++;
if (hc->chunk_num == 256) {
return tlen;
}
x = hc->chunk_num;
br_hmac_init(&hmac_ctx, &hc->u.prk_ctx, 0);
if (x != 1) {
br_hmac_update(&hmac_ctx, hc->buf, hc->dig_len);
}
br_hmac_update(&hmac_ctx, info, info_len);
br_hmac_update(&hmac_ctx, &x, 1);
br_hmac_out(&hmac_ctx, hc->buf);
hc->ptr = 0;
}
clen = hc->dig_len - hc->ptr;
if (clen > out_len) {
clen = out_len;
}
memcpy(out, hc->buf + hc->ptr, clen);
out = (unsigned char *)out + clen;
out_len -= clen;
hc->ptr += clen;
tlen += clen;
}
return tlen;
}
