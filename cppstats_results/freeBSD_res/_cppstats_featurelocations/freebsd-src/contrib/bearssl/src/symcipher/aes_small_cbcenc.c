























#include "inner.h"


void
br_aes_small_cbcenc_init(br_aes_small_cbcenc_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_small_cbcenc_vtable;
ctx->num_rounds = br_aes_keysched(ctx->skey, key, len);
}


void
br_aes_small_cbcenc_run(const br_aes_small_cbcenc_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf, *ivbuf;

ivbuf = iv;
buf = data;
while (len > 0) {
int i;

for (i = 0; i < 16; i ++) {
buf[i] ^= ivbuf[i];
}
br_aes_small_encrypt(ctx->num_rounds, ctx->skey, buf);
memcpy(ivbuf, buf, 16);
buf += 16;
len -= 16;
}
}


const br_block_cbcenc_class br_aes_small_cbcenc_vtable = {
sizeof(br_aes_small_cbcenc_keys),
16,
4,
(void (*)(const br_block_cbcenc_class **, const void *, size_t))
&br_aes_small_cbcenc_init,
(void (*)(const br_block_cbcenc_class *const *, void *, void *, size_t))
&br_aes_small_cbcenc_run
};
