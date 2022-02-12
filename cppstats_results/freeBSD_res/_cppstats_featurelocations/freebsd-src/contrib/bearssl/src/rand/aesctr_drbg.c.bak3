























#include "inner.h"


void
br_aesctr_drbg_init(br_aesctr_drbg_context *ctx,
const br_block_ctr_class *aesctr,
const void *seed, size_t len)
{
unsigned char tmp[16];

ctx->vtable = &br_aesctr_drbg_vtable;
memset(tmp, 0, sizeof tmp);
aesctr->init(&ctx->sk.vtable, tmp, 16);
ctx->cc = 0;
br_aesctr_drbg_update(ctx, seed, len);
}


void
br_aesctr_drbg_generate(br_aesctr_drbg_context *ctx, void *out, size_t len)
{
unsigned char *buf;
unsigned char iv[12];

buf = out;
memset(iv, 0, sizeof iv);
while (len > 0) {
size_t clen;







clen = len;
if (clen > 65280) {
clen = 65280;
}





if ((uint32_t)(ctx->cc + ((clen + 15) >> 4)) > 32768) {
clen = (32768 - ctx->cc) << 4;
if (clen > len) {
clen = len;
}
}




memset(buf, 0, clen);
ctx->cc = ctx->sk.vtable->run(&ctx->sk.vtable,
iv, ctx->cc, buf, clen);
buf += clen;
len -= clen;




if (ctx->cc >= 32768) {
br_aesctr_drbg_update(ctx, NULL, 0);
}
}
}


void
br_aesctr_drbg_update(br_aesctr_drbg_context *ctx, const void *seed, size_t len)
{




























unsigned char s[16], iv[12];
unsigned char G[16], H[16];
int first;





memset(iv, 0xFF, sizeof iv);
memset(s, 0, 16);
ctx->sk.vtable->run(&ctx->sk.vtable, iv, 0xFFFFFFFF, s, 16);




memset(G, 0xB6, sizeof G);
memset(H, 0x5A, sizeof H);





first = 1;
for (;;) {
unsigned char tmp[32];
unsigned char newG[16];




memcpy(tmp, H, 16);
if (first) {
memcpy(tmp + 16, s, 16);
first = 0;
} else {
size_t clen;

if (len == 0) {
break;
}
clen = len < 16 ? len : 16;
memcpy(tmp + 16, seed, clen);
memset(tmp + 16 + clen, 0, 16 - clen);
seed = (const unsigned char *)seed + clen;
len -= clen;
}
ctx->sk.vtable->init(&ctx->sk.vtable, tmp, 32);




memcpy(iv, G, 12);
memcpy(newG, G, 16);
ctx->sk.vtable->run(&ctx->sk.vtable, iv,
br_dec32be(G + 12), newG, 16);
iv[0] ^= 0x01;
memcpy(H, G, 16);
H[0] ^= 0x01;
ctx->sk.vtable->run(&ctx->sk.vtable, iv,
br_dec32be(G + 12), H, 16);
memcpy(G, newG, 16);
}





ctx->sk.vtable->init(&ctx->sk.vtable, H, 16);
ctx->cc = 0;
}


const br_prng_class br_aesctr_drbg_vtable = {
sizeof(br_aesctr_drbg_context),
(void (*)(const br_prng_class **, const void *, const void *, size_t))
&br_aesctr_drbg_init,
(void (*)(const br_prng_class **, void *, size_t))
&br_aesctr_drbg_generate,
(void (*)(const br_prng_class **, const void *, size_t))
&br_aesctr_drbg_update
};
