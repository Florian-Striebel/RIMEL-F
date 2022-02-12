























#include "inner.h"


void
br_aes_small_ctr_init(br_aes_small_ctr_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_small_ctr_vtable;
ctx->num_rounds = br_aes_keysched(ctx->skey, key, len);
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
br_aes_small_ctr_run(const br_aes_small_ctr_keys *ctx,
const void *iv, uint32_t cc, void *data, size_t len)
{
unsigned char *buf;

buf = data;
while (len > 0) {
unsigned char tmp[16];

memcpy(tmp, iv, 12);
br_enc32be(tmp + 12, cc ++);
br_aes_small_encrypt(ctx->num_rounds, ctx->skey, tmp);
if (len <= 16) {
xorbuf(buf, tmp, len);
break;
}
xorbuf(buf, tmp, 16);
buf += 16;
len -= 16;
}
return cc;
}


const br_block_ctr_class br_aes_small_ctr_vtable = {
sizeof(br_aes_small_ctr_keys),
16,
4,
(void (*)(const br_block_ctr_class **, const void *, size_t))
&br_aes_small_ctr_init,
(uint32_t (*)(const br_block_ctr_class *const *,
const void *, uint32_t, void *, size_t))
&br_aes_small_ctr_run
};
