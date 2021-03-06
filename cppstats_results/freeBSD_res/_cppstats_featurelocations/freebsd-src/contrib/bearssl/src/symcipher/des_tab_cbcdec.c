























#include "inner.h"


void
br_des_tab_cbcdec_init(br_des_tab_cbcdec_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_des_tab_cbcdec_vtable;
ctx->num_rounds = br_des_tab_keysched(ctx->skey, key, len);
if (len == 8) {
br_des_rev_skey(ctx->skey);
} else {
int i;

for (i = 0; i < 48; i += 2) {
uint32_t t;

t = ctx->skey[i];
ctx->skey[i] = ctx->skey[94 - i];
ctx->skey[94 - i] = t;
t = ctx->skey[i + 1];
ctx->skey[i + 1] = ctx->skey[95 - i];
ctx->skey[95 - i] = t;
}
}
}


void
br_des_tab_cbcdec_run(const br_des_tab_cbcdec_keys *ctx,
void *iv, void *data, size_t len)
{
unsigned char *buf, *ivbuf;

ivbuf = iv;
buf = data;
while (len > 0) {
unsigned char tmp[8];
int i;

memcpy(tmp, buf, 8);
br_des_tab_process_block(ctx->num_rounds, ctx->skey, buf);
for (i = 0; i < 8; i ++) {
buf[i] ^= ivbuf[i];
}
memcpy(ivbuf, tmp, 8);
buf += 8;
len -= 8;
}
}


const br_block_cbcdec_class br_des_tab_cbcdec_vtable = {
sizeof(br_des_tab_cbcdec_keys),
8,
3,
(void (*)(const br_block_cbcdec_class **, const void *, size_t))
&br_des_tab_cbcdec_init,
(void (*)(const br_block_cbcdec_class *const *, void *, void *, size_t))
&br_des_tab_cbcdec_run
};
