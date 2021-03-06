























#include "inner.h"
























void
br_ccm_init(br_ccm_context *ctx, const br_block_ctrcbc_class **bctx)
{
ctx->bctx = bctx;
}


int
br_ccm_reset(br_ccm_context *ctx, const void *nonce, size_t nonce_len,
uint64_t aad_len, uint64_t data_len, size_t tag_len)
{
unsigned char tmp[16];
unsigned u, q;

if (nonce_len < 7 || nonce_len > 13) {
return 0;
}
if (tag_len < 4 || tag_len > 16 || (tag_len & 1) != 0) {
return 0;
}
q = 15 - (unsigned)nonce_len;
ctx->tag_len = tag_len;




tmp[0] = (aad_len > 0 ? 0x40 : 0x00)
| (((unsigned)tag_len - 2) << 2)
| (q - 1);
memcpy(tmp + 1, nonce, nonce_len);
for (u = 0; u < q; u ++) {
tmp[15 - u] = (unsigned char)data_len;
data_len >>= 8;
}
if (data_len != 0) {





return 0;
}




memset(ctx->cbcmac, 0, sizeof ctx->cbcmac);
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac, tmp, sizeof tmp);




if ((aad_len >> 32) != 0) {
ctx->buf[0] = 0xFF;
ctx->buf[1] = 0xFF;
br_enc64be(ctx->buf + 2, aad_len);
ctx->ptr = 10;
} else if (aad_len >= 0xFF00) {
ctx->buf[0] = 0xFF;
ctx->buf[1] = 0xFE;
br_enc32be(ctx->buf + 2, (uint32_t)aad_len);
ctx->ptr = 6;
} else if (aad_len > 0) {
br_enc16be(ctx->buf, (unsigned)aad_len);
ctx->ptr = 2;
} else {
ctx->ptr = 0;
}




ctx->ctr[0] = q - 1;
memcpy(ctx->ctr + 1, nonce, nonce_len);
memset(ctx->ctr + 1 + nonce_len, 0, q);
memset(ctx->tagmask, 0, sizeof ctx->tagmask);
(*ctx->bctx)->ctr(ctx->bctx, ctx->ctr,
ctx->tagmask, sizeof ctx->tagmask);

return 1;
}


void
br_ccm_aad_inject(br_ccm_context *ctx, const void *data, size_t len)
{
const unsigned char *dbuf;
size_t ptr;

dbuf = data;




ptr = ctx->ptr;
if (ptr != 0) {
size_t clen;

clen = (sizeof ctx->buf) - ptr;
if (clen > len) {
memcpy(ctx->buf + ptr, dbuf, len);
ctx->ptr = ptr + len;
return;
}
memcpy(ctx->buf + ptr, dbuf, clen);
dbuf += clen;
len -= clen;
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac,
ctx->buf, sizeof ctx->buf);
}




ptr = len & 15;
len -= ptr;
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac, dbuf, len);
dbuf += len;




memcpy(ctx->buf, dbuf, ptr);
ctx->ptr = ptr;
}


void
br_ccm_flip(br_ccm_context *ctx)
{
size_t ptr;




ptr = ctx->ptr;
if (ptr != 0) {
memset(ctx->buf + ptr, 0, (sizeof ctx->buf) - ptr);
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac,
ctx->buf, sizeof ctx->buf);
ctx->ptr = 0;
}




}


void
br_ccm_run(br_ccm_context *ctx, int encrypt, void *data, size_t len)
{
unsigned char *dbuf;
size_t ptr;

dbuf = data;






ptr = ctx->ptr;
if (ptr != 0) {
size_t clen;
size_t u;

clen = (sizeof ctx->buf) - ptr;
if (clen > len) {
clen = len;
}
if (encrypt) {
for (u = 0; u < clen; u ++) {
unsigned w, x;

w = ctx->buf[ptr + u];
x = dbuf[u];
ctx->buf[ptr + u] = x;
dbuf[u] = w ^ x;
}
} else {
for (u = 0; u < clen; u ++) {
unsigned w;

w = ctx->buf[ptr + u] ^ dbuf[u];
dbuf[u] = w;
ctx->buf[ptr + u] = w;
}
}
dbuf += clen;
len -= clen;
ptr += clen;
if (ptr < sizeof ctx->buf) {
ctx->ptr = ptr;
return;
}
(*ctx->bctx)->mac(ctx->bctx,
ctx->cbcmac, ctx->buf, sizeof ctx->buf);
}












ptr = len & 15;
len -= ptr;
if (encrypt) {
(*ctx->bctx)->decrypt(ctx->bctx, ctx->ctr, ctx->cbcmac,
dbuf, len);
} else {
(*ctx->bctx)->encrypt(ctx->bctx, ctx->ctr, ctx->cbcmac,
dbuf, len);
}
dbuf += len;





if (ptr != 0) {
size_t u;

memset(ctx->buf, 0, sizeof ctx->buf);
(*ctx->bctx)->ctr(ctx->bctx, ctx->ctr,
ctx->buf, sizeof ctx->buf);
if (encrypt) {
for (u = 0; u < ptr; u ++) {
unsigned w, x;

w = ctx->buf[u];
x = dbuf[u];
ctx->buf[u] = x;
dbuf[u] = w ^ x;
}
} else {
for (u = 0; u < ptr; u ++) {
unsigned w;

w = ctx->buf[u] ^ dbuf[u];
dbuf[u] = w;
ctx->buf[u] = w;
}
}
}
ctx->ptr = ptr;
}


size_t
br_ccm_get_tag(br_ccm_context *ctx, void *tag)
{
size_t ptr;
size_t u;





ptr = ctx->ptr;
if (ptr != 0) {
memset(ctx->buf + ptr, 0, (sizeof ctx->buf) - ptr);
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac,
ctx->buf, sizeof ctx->buf);
}




for (u = 0; u < ctx->tag_len; u ++) {
ctx->cbcmac[u] ^= ctx->tagmask[u];
}
memcpy(tag, ctx->cbcmac, ctx->tag_len);
return ctx->tag_len;
}


uint32_t
br_ccm_check_tag(br_ccm_context *ctx, const void *tag)
{
unsigned char tmp[16];
size_t u, tag_len;
uint32_t z;

tag_len = br_ccm_get_tag(ctx, tmp);
z = 0;
for (u = 0; u < tag_len; u ++) {
z |= tmp[u] ^ ((const unsigned char *)tag)[u];
}
return EQ0(z);
}
