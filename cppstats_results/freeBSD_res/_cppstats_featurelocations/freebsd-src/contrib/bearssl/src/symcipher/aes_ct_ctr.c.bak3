























#include "inner.h"


void
br_aes_ct_ctr_init(br_aes_ct_ctr_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_ct_ctr_vtable;
ctx->num_rounds = br_aes_ct_keysched(ctx->skey, key, len);
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
br_aes_ct_ctr_run(const br_aes_ct_ctr_keys *ctx,
const void *iv, uint32_t cc, void *data, size_t len)
{
unsigned char *buf;
const unsigned char *ivbuf;
uint32_t iv0, iv1, iv2;
uint32_t sk_exp[120];

br_aes_ct_skey_expand(sk_exp, ctx->num_rounds, ctx->skey);
ivbuf = iv;
iv0 = br_dec32le(ivbuf);
iv1 = br_dec32le(ivbuf + 4);
iv2 = br_dec32le(ivbuf + 8);
buf = data;
while (len > 0) {
uint32_t q[8];
unsigned char tmp[32];





q[0] = q[1] = iv0;
q[2] = q[3] = iv1;
q[4] = q[5] = iv2;
q[6] = br_swap32(cc);
q[7] = br_swap32(cc + 1);
br_aes_ct_ortho(q);
br_aes_ct_bitslice_encrypt(ctx->num_rounds, sk_exp, q);
br_aes_ct_ortho(q);
br_enc32le(tmp, q[0]);
br_enc32le(tmp + 4, q[2]);
br_enc32le(tmp + 8, q[4]);
br_enc32le(tmp + 12, q[6]);
br_enc32le(tmp + 16, q[1]);
br_enc32le(tmp + 20, q[3]);
br_enc32le(tmp + 24, q[5]);
br_enc32le(tmp + 28, q[7]);

if (len <= 32) {
xorbuf(buf, tmp, len);
cc ++;
if (len > 16) {
cc ++;
}
break;
}
xorbuf(buf, tmp, 32);
buf += 32;
len -= 32;
cc += 2;
}
return cc;
}


const br_block_ctr_class br_aes_ct_ctr_vtable = {
sizeof(br_aes_ct_ctr_keys),
16,
4,
(void (*)(const br_block_ctr_class **, const void *, size_t))
&br_aes_ct_ctr_init,
(uint32_t (*)(const br_block_ctr_class *const *,
const void *, uint32_t, void *, size_t))
&br_aes_ct_ctr_run
};
