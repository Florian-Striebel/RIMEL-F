























#include "inner.h"


void
br_aes_ct64_cbcdec_init(br_aes_ct64_cbcdec_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_ct64_cbcdec_vtable;
ctx->num_rounds = br_aes_ct64_keysched(ctx->skey, key, len);
}


void
br_aes_ct64_cbcdec_run(const br_aes_ct64_cbcdec_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf;
uint64_t sk_exp[120];
uint32_t ivw[4];

br_aes_ct64_skey_expand(sk_exp, ctx->num_rounds, ctx->skey);
br_range_dec32le(ivw, 4, iv);
buf = data;
while (len > 0) {
uint64_t q[8];
uint32_t w1[16], w2[16];
int i;

if (len >= 64) {
br_range_dec32le(w1, 16, buf);
} else {
br_range_dec32le(w1, len >> 2, buf);
}
for (i = 0; i < 4; i ++) {
br_aes_ct64_interleave_in(
&q[i], &q[i + 4], w1 + (i << 2));
}
br_aes_ct64_ortho(q);
br_aes_ct64_bitslice_decrypt(ctx->num_rounds, sk_exp, q);
br_aes_ct64_ortho(q);
for (i = 0; i < 4; i ++) {
br_aes_ct64_interleave_out(
w2 + (i << 2), q[i], q[i + 4]);
}
for (i = 0; i < 4; i ++) {
w2[i] ^= ivw[i];
}
if (len >= 64) {
for (i = 4; i < 16; i ++) {
w2[i] ^= w1[i - 4];
}
memcpy(ivw, w1 + 12, sizeof ivw);
br_range_enc32le(buf, w2, 16);
} else {
int j;

j = (int)(len >> 2);
for (i = 4; i < j; i ++) {
w2[i] ^= w1[i - 4];
}
memcpy(ivw, w1 + j - 4, sizeof ivw);
br_range_enc32le(buf, w2, j);
break;
}
buf += 64;
len -= 64;
}
br_range_enc32le(iv, ivw, 4);
}


const br_block_cbcdec_class br_aes_ct64_cbcdec_vtable = {
sizeof(br_aes_ct64_cbcdec_keys),
16,
4,
(void (*)(const br_block_cbcdec_class **, const void *, size_t))
&br_aes_ct64_cbcdec_init,
(void (*)(const br_block_cbcdec_class *const *, void *, void *, size_t))
&br_aes_ct64_cbcdec_run
};
