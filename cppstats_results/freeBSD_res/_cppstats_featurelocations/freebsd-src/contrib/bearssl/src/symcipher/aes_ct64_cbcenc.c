























#include "inner.h"


void
br_aes_ct64_cbcenc_init(br_aes_ct64_cbcenc_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_ct64_cbcenc_vtable;
ctx->num_rounds = br_aes_ct64_keysched(ctx->skey, key, len);
}


void
br_aes_ct64_cbcenc_run(const br_aes_ct64_cbcenc_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf;
uint64_t sk_exp[120];
uint32_t ivw[4];

br_aes_ct64_skey_expand(sk_exp, ctx->num_rounds, ctx->skey);
br_range_dec32le(ivw, 4, iv);
buf = data;
while (len > 0) {
uint32_t w[4];
uint64_t q[8];

w[0] = ivw[0] ^ br_dec32le(buf);
w[1] = ivw[1] ^ br_dec32le(buf + 4);
w[2] = ivw[2] ^ br_dec32le(buf + 8);
w[3] = ivw[3] ^ br_dec32le(buf + 12);
br_aes_ct64_interleave_in(&q[0], &q[4], w);
br_aes_ct64_ortho(q);
br_aes_ct64_bitslice_encrypt(ctx->num_rounds, sk_exp, q);
br_aes_ct64_ortho(q);
br_aes_ct64_interleave_out(w, q[0], q[4]);
memcpy(ivw, w, sizeof w);
br_enc32le(buf, w[0]);
br_enc32le(buf + 4, w[1]);
br_enc32le(buf + 8, w[2]);
br_enc32le(buf + 12, w[3]);
buf += 16;
len -= 16;
}
br_range_enc32le(iv, ivw, 4);
}


const br_block_cbcenc_class br_aes_ct64_cbcenc_vtable = {
sizeof(br_aes_ct64_cbcenc_keys),
16,
4,
(void (*)(const br_block_cbcenc_class **, const void *, size_t))
&br_aes_ct64_cbcenc_init,
(void (*)(const br_block_cbcenc_class *const *, void *, void *, size_t))
&br_aes_ct64_cbcenc_run
};
