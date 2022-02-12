























#include "inner.h"


void
br_aes_ct64_ctr_init(br_aes_ct64_ctr_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_ct64_ctr_vtable;
ctx->num_rounds = br_aes_ct64_keysched(ctx->skey, key, len);
}

static void
xorbuf(void *dst, const void *src, size_t len)
{
unsigned char *d;
const unsigned char *s;

d = dst;
s = src;
while (len -- > 0) {
*d ++ ^= *s ++;
}
}


uint32_t
br_aes_ct64_ctr_run(const br_aes_ct64_ctr_keys *ctx,
const void *iv, uint32_t cc, void *data, size_t len)
{
unsigned char *buf;
uint32_t ivw[16];
uint64_t sk_exp[120];

br_aes_ct64_skey_expand(sk_exp, ctx->num_rounds, ctx->skey);
br_range_dec32le(ivw, 3, iv);
memcpy(ivw + 4, ivw, 3 * sizeof(uint32_t));
memcpy(ivw + 8, ivw, 3 * sizeof(uint32_t));
memcpy(ivw + 12, ivw, 3 * sizeof(uint32_t));
buf = data;
while (len > 0) {
uint64_t q[8];
uint32_t w[16];
unsigned char tmp[64];
int i;





memcpy(w, ivw, sizeof ivw);
w[3] = br_swap32(cc);
w[7] = br_swap32(cc + 1);
w[11] = br_swap32(cc + 2);
w[15] = br_swap32(cc + 3);
for (i = 0; i < 4; i ++) {
br_aes_ct64_interleave_in(
&q[i], &q[i + 4], w + (i << 2));
}
br_aes_ct64_ortho(q);
br_aes_ct64_bitslice_encrypt(ctx->num_rounds, sk_exp, q);
br_aes_ct64_ortho(q);
for (i = 0; i < 4; i ++) {
br_aes_ct64_interleave_out(
w + (i << 2), q[i], q[i + 4]);
}
br_range_enc32le(tmp, w, 16);
if (len <= 64) {
xorbuf(buf, tmp, len);
cc += (uint32_t)len >> 4;
break;
}
xorbuf(buf, tmp, 64);
buf += 64;
len -= 64;
cc += 4;
}
return cc;
}


const br_block_ctr_class br_aes_ct64_ctr_vtable = {
sizeof(br_aes_ct64_ctr_keys),
16,
4,
(void (*)(const br_block_ctr_class **, const void *, size_t))
&br_aes_ct64_ctr_init,
(uint32_t (*)(const br_block_ctr_class *const *,
const void *, uint32_t, void *, size_t))
&br_aes_ct64_ctr_run
};
