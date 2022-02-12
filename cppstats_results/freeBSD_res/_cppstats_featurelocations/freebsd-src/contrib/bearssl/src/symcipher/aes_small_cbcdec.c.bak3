























#include "inner.h"


void
br_aes_small_cbcdec_init(br_aes_small_cbcdec_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_small_cbcdec_vtable;
ctx->num_rounds = br_aes_keysched(ctx->skey, key, len);
}


void
br_aes_small_cbcdec_run(const br_aes_small_cbcdec_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf, *ivbuf;

ivbuf = iv;
buf = data;
while (len > 0) {
unsigned char tmp[16];
int i;

memcpy(tmp, buf, 16);
br_aes_small_decrypt(ctx->num_rounds, ctx->skey, buf);
for (i = 0; i < 16; i ++) {
buf[i] ^= ivbuf[i];
}
memcpy(ivbuf, tmp, 16);
buf += 16;
len -= 16;
}
}


const br_block_cbcdec_class br_aes_small_cbcdec_vtable = {
sizeof(br_aes_small_cbcdec_keys),
16,
4,
(void (*)(const br_block_cbcdec_class **, const void *, size_t))
&br_aes_small_cbcdec_init,
(void (*)(const br_block_cbcdec_class *const *, void *, void *, size_t))
&br_aes_small_cbcdec_run
};
