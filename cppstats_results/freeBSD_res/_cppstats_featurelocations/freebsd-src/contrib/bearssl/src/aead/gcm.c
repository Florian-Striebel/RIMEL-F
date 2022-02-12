























#include "inner.h"




























void
br_gcm_init(br_gcm_context *ctx, const br_block_ctr_class **bctx, br_ghash gh)
{
unsigned char iv[12];

ctx->vtable = &br_gcm_vtable;
ctx->bctx = bctx;
ctx->gh = gh;







memset(ctx->h, 0, sizeof ctx->h);
memset(iv, 0, sizeof iv);
(*bctx)->run(bctx, iv, 0, ctx->h, sizeof ctx->h);
}


void
br_gcm_reset(br_gcm_context *ctx, const void *iv, size_t len)
{









if (len == 12) {
memcpy(ctx->j0_1, iv, 12);
ctx->j0_2 = 1;
} else {
unsigned char ty[16], tmp[16];

memset(ty, 0, sizeof ty);
ctx->gh(ty, ctx->h, iv, len);
memset(tmp, 0, 8);
br_enc64be(tmp + 8, (uint64_t)len << 3);
ctx->gh(ty, ctx->h, tmp, 16);
memcpy(ctx->j0_1, ty, 12);
ctx->j0_2 = br_dec32be(ty + 12);
}
ctx->jc = ctx->j0_2 + 1;
memset(ctx->y, 0, sizeof ctx->y);
ctx->count_aad = 0;
ctx->count_ctr = 0;
}


void
br_gcm_aad_inject(br_gcm_context *ctx, const void *data, size_t len)
{
size_t ptr, dlen;

ptr = (size_t)ctx->count_aad & (size_t)15;
if (ptr != 0) {




size_t clen;

clen = 16 - ptr;
if (len < clen) {
memcpy(ctx->buf + ptr, data, len);
ctx->count_aad += (uint64_t)len;
return;
}
memcpy(ctx->buf + ptr, data, clen);
ctx->gh(ctx->y, ctx->h, ctx->buf, 16);
data = (const unsigned char *)data + clen;
len -= clen;
ctx->count_aad += (uint64_t)clen;
}






dlen = len & ~(size_t)15;
ctx->gh(ctx->y, ctx->h, data, dlen);
memcpy(ctx->buf, (const unsigned char *)data + dlen, len - dlen);
ctx->count_aad += (uint64_t)len;
}


void
br_gcm_flip(br_gcm_context *ctx)
{





size_t ptr;

ptr = (size_t)ctx->count_aad & (size_t)15;
if (ptr != 0) {
ctx->gh(ctx->y, ctx->h, ctx->buf, ptr);
}
}


void
br_gcm_run(br_gcm_context *ctx, int encrypt, void *data, size_t len)
{
unsigned char *buf;
size_t ptr, dlen;

buf = data;
ptr = (size_t)ctx->count_ctr & (size_t)15;
if (ptr != 0) {



size_t u, clen;

clen = 16 - ptr;
if (len < clen) {
clen = len;
}
for (u = 0; u < clen; u ++) {
unsigned x, y;

x = buf[u];
y = x ^ ctx->buf[ptr + u];
ctx->buf[ptr + u] = encrypt ? y : x;
buf[u] = y;
}
ctx->count_ctr += (uint64_t)clen;
buf += clen;
len -= clen;
if (ptr + clen < 16) {
return;
}
ctx->gh(ctx->y, ctx->h, ctx->buf, 16);
}




dlen = len & ~(size_t)15;
if (!encrypt) {
ctx->gh(ctx->y, ctx->h, buf, dlen);
}
ctx->jc = (*ctx->bctx)->run(ctx->bctx, ctx->j0_1, ctx->jc, buf, dlen);
if (encrypt) {
ctx->gh(ctx->y, ctx->h, buf, dlen);
}
buf += dlen;
len -= dlen;
ctx->count_ctr += (uint64_t)dlen;

if (len > 0) {



size_t u;

memset(ctx->buf, 0, sizeof ctx->buf);
ctx->jc = (*ctx->bctx)->run(ctx->bctx, ctx->j0_1,
ctx->jc, ctx->buf, 16);
for (u = 0; u < len; u ++) {
unsigned x, y;

x = buf[u];
y = x ^ ctx->buf[u];
ctx->buf[u] = encrypt ? y : x;
buf[u] = y;
}
ctx->count_ctr += (uint64_t)len;
}
}


void
br_gcm_get_tag(br_gcm_context *ctx, void *tag)
{
size_t ptr;
unsigned char tmp[16];

ptr = (size_t)ctx->count_ctr & (size_t)15;
if (ptr > 0) {





ctx->gh(ctx->y, ctx->h, ctx->buf, ptr);
}




br_enc64be(tmp, ctx->count_aad << 3);
br_enc64be(tmp + 8, ctx->count_ctr << 3);
ctx->gh(ctx->y, ctx->h, tmp, 16);





memcpy(tag, ctx->y, 16);
(*ctx->bctx)->run(ctx->bctx, ctx->j0_1, ctx->j0_2, tag, 16);
}


void
br_gcm_get_tag_trunc(br_gcm_context *ctx, void *tag, size_t len)
{
unsigned char tmp[16];

br_gcm_get_tag(ctx, tmp);
memcpy(tag, tmp, len);
}


uint32_t
br_gcm_check_tag_trunc(br_gcm_context *ctx, const void *tag, size_t len)
{
unsigned char tmp[16];
size_t u;
int x;

br_gcm_get_tag(ctx, tmp);
x = 0;
for (u = 0; u < len; u ++) {
x |= tmp[u] ^ ((const unsigned char *)tag)[u];
}
return EQ0(x);
}


uint32_t
br_gcm_check_tag(br_gcm_context *ctx, const void *tag)
{
return br_gcm_check_tag_trunc(ctx, tag, 16);
}


const br_aead_class br_gcm_vtable = {
16,
(void (*)(const br_aead_class **, const void *, size_t))
&br_gcm_reset,
(void (*)(const br_aead_class **, const void *, size_t))
&br_gcm_aad_inject,
(void (*)(const br_aead_class **))
&br_gcm_flip,
(void (*)(const br_aead_class **, int, void *, size_t))
&br_gcm_run,
(void (*)(const br_aead_class **, void *))
&br_gcm_get_tag,
(uint32_t (*)(const br_aead_class **, const void *))
&br_gcm_check_tag,
(void (*)(const br_aead_class **, void *, size_t))
&br_gcm_get_tag_trunc,
(uint32_t (*)(const br_aead_class **, const void *, size_t))
&br_gcm_check_tag_trunc
};
