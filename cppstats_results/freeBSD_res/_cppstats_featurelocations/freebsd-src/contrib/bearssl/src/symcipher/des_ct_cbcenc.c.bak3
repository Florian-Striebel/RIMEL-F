























#include "inner.h"


void
br_des_ct_cbcenc_init(br_des_ct_cbcenc_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_des_ct_cbcenc_vtable;
ctx->num_rounds = br_des_ct_keysched(ctx->skey, key, len);
}


void
br_des_ct_cbcenc_run(const br_des_ct_cbcenc_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf, *ivbuf;
uint32_t sk_exp[288];

br_des_ct_skey_expand(sk_exp, ctx->num_rounds, ctx->skey);
ivbuf = iv;
buf = data;
while (len > 0) {
int i;

for (i = 0; i < 8; i ++) {
buf[i] ^= ivbuf[i];
}
br_des_ct_process_block(ctx->num_rounds, sk_exp, buf);
memcpy(ivbuf, buf, 8);
buf += 8;
len -= 8;
}
}


const br_block_cbcenc_class br_des_ct_cbcenc_vtable = {
sizeof(br_des_ct_cbcenc_keys),
8,
3,
(void (*)(const br_block_cbcenc_class **, const void *, size_t))
&br_des_ct_cbcenc_init,
(void (*)(const br_block_cbcenc_class *const *, void *, void *, size_t))
&br_des_ct_cbcenc_run
};
