























#include "inner.h"




































static void
omac_start(br_eax_context *ctx, unsigned val)
{
memset(ctx->cbcmac, 0, sizeof ctx->cbcmac);
memset(ctx->buf, 0, sizeof ctx->buf);
ctx->buf[15] = val;
ctx->ptr = 16;
}





static void
double_gf128(unsigned char *dst, const unsigned char *src)
{
unsigned cc;
int i;

cc = 0x87 & -((unsigned)src[0] >> 7);
for (i = 15; i >= 0; i --) {
unsigned z;

z = (src[i] << 1) ^ cc;
cc = z >> 8;
dst[i] = (unsigned char)z;
}
}





static void
do_pad(br_eax_context *ctx)
{
unsigned char *pad;
size_t ptr, u;

ptr = ctx->ptr;
if (ptr == 16) {
pad = ctx->L2;
} else {
ctx->buf[ptr ++] = 0x80;
memset(ctx->buf + ptr, 0x00, 16 - ptr);
pad = ctx->L4;
}
for (u = 0; u < sizeof ctx->buf; u ++) {
ctx->buf[u] ^= pad[u];
}
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac, ctx->buf, sizeof ctx->buf);
}













static void
do_cbcmac_chunk(br_eax_context *ctx, const void *data, size_t len)
{
size_t ptr;

if (len == 0) {
return;
}
ptr = len & (size_t)15;
if (ptr == 0) {
len -= 16;
ptr = 16;
} else {
len -= ptr;
}
if (ctx->ptr == 16) {
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac,
ctx->buf, sizeof ctx->buf);
}
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac, data, len);
memcpy(ctx->buf, (const unsigned char *)data + len, ptr);
ctx->ptr = ptr;
}


void
br_eax_init(br_eax_context *ctx, const br_block_ctrcbc_class **bctx)
{
unsigned char tmp[16], iv[16];

ctx->vtable = &br_eax_vtable;
ctx->bctx = bctx;




memset(tmp, 0, sizeof tmp);
memset(iv, 0, sizeof iv);
(*bctx)->ctr(bctx, iv, tmp, sizeof tmp);
double_gf128(ctx->L2, tmp);
double_gf128(ctx->L4, ctx->L2);
}


void
br_eax_capture(const br_eax_context *ctx, br_eax_state *st)
{





int i;

memset(st->st, 0, sizeof st->st);
for (i = 0; i < 3; i ++) {
unsigned char tmp[16];

memset(tmp, 0, sizeof tmp);
tmp[15] = (unsigned char)i;
(*ctx->bctx)->mac(ctx->bctx, st->st[i], tmp, sizeof tmp);
}
}


void
br_eax_reset(br_eax_context *ctx, const void *nonce, size_t len)
{



omac_start(ctx, 0);
do_cbcmac_chunk(ctx, nonce, len);
do_pad(ctx);
memcpy(ctx->nonce, ctx->cbcmac, sizeof ctx->cbcmac);




omac_start(ctx, 1);





ctx->head[0] = 0;
}


void
br_eax_reset_pre_aad(br_eax_context *ctx, const br_eax_state *st,
const void *nonce, size_t len)
{
if (len == 0) {
omac_start(ctx, 0);
} else {
memcpy(ctx->cbcmac, st->st[0], sizeof ctx->cbcmac);
ctx->ptr = 0;
do_cbcmac_chunk(ctx, nonce, len);
}
do_pad(ctx);
memcpy(ctx->nonce, ctx->cbcmac, sizeof ctx->cbcmac);

memcpy(ctx->cbcmac, st->st[1], sizeof ctx->cbcmac);
ctx->ptr = 0;

memcpy(ctx->ctr, st->st[2], sizeof ctx->ctr);






ctx->head[0] = 1;
}


void
br_eax_reset_post_aad(br_eax_context *ctx, const br_eax_state *st,
const void *nonce, size_t len)
{
if (len == 0) {
omac_start(ctx, 0);
} else {
memcpy(ctx->cbcmac, st->st[0], sizeof ctx->cbcmac);
ctx->ptr = 0;
do_cbcmac_chunk(ctx, nonce, len);
}
do_pad(ctx);
memcpy(ctx->nonce, ctx->cbcmac, sizeof ctx->cbcmac);
memcpy(ctx->ctr, ctx->nonce, sizeof ctx->nonce);

memcpy(ctx->head, st->st[1], sizeof ctx->head);

memcpy(ctx->cbcmac, st->st[2], sizeof ctx->cbcmac);
ctx->ptr = 0;
}


void
br_eax_aad_inject(br_eax_context *ctx, const void *data, size_t len)
{
size_t ptr;

ptr = ctx->ptr;




if (ptr < 16) {
size_t clen;

clen = 16 - ptr;
if (len <= clen) {
memcpy(ctx->buf + ptr, data, len);
ctx->ptr = ptr + len;
return;
}
memcpy(ctx->buf + ptr, data, clen);
data = (const unsigned char *)data + clen;
len -= clen;
}





do_cbcmac_chunk(ctx, data, len);
}


void
br_eax_flip(br_eax_context *ctx)
{
int from_capture;






from_capture = ctx->head[0];




do_pad(ctx);
memcpy(ctx->head, ctx->cbcmac, sizeof ctx->cbcmac);






if (from_capture) {
memcpy(ctx->cbcmac, ctx->ctr, sizeof ctx->cbcmac);
ctx->ptr = 0;
} else {
omac_start(ctx, 2);
}




memcpy(ctx->ctr, ctx->nonce, sizeof ctx->nonce);
}


void
br_eax_run(br_eax_context *ctx, int encrypt, void *data, size_t len)
{
unsigned char *dbuf;
size_t ptr;




if (len == 0) {
return;
}

dbuf = data;
ptr = ctx->ptr;






if (ptr != 0 && ptr != 16) {



size_t u, clen;

clen = 16 - ptr;
if (len <= clen) {
clen = len;
}
if (encrypt) {
for (u = 0; u < clen; u ++) {
ctx->buf[ptr + u] ^= dbuf[u];
}
memcpy(dbuf, ctx->buf + ptr, clen);
} else {
for (u = 0; u < clen; u ++) {
unsigned dx, sx;

sx = ctx->buf[ptr + u];
dx = dbuf[u];
ctx->buf[ptr + u] = dx;
dbuf[u] = sx ^ dx;
}
}

if (len <= clen) {
ctx->ptr = ptr + clen;
return;
}
dbuf += clen;
len -= clen;
}






if (ptr != 0) {
(*ctx->bctx)->mac(ctx->bctx, ctx->cbcmac,
ctx->buf, sizeof ctx->buf);
}





ptr = len & (size_t)15;
if (ptr == 0) {
len -= 16;
ptr = 16;
} else {
len -= ptr;
}
if (encrypt) {
(*ctx->bctx)->encrypt(ctx->bctx, ctx->ctr, ctx->cbcmac,
dbuf, len);
} else {
(*ctx->bctx)->decrypt(ctx->bctx, ctx->ctr, ctx->cbcmac,
dbuf, len);
}
dbuf += len;





memset(ctx->buf, 0, sizeof ctx->buf);
(*ctx->bctx)->ctr(ctx->bctx, ctx->ctr, ctx->buf, sizeof ctx->buf);
if (encrypt) {
size_t u;

for (u = 0; u < ptr; u ++) {
ctx->buf[u] ^= dbuf[u];
}
memcpy(dbuf, ctx->buf, ptr);
} else {
size_t u;

for (u = 0; u < ptr; u ++) {
unsigned dx, sx;

sx = ctx->buf[u];
dx = dbuf[u];
ctx->buf[u] = dx;
dbuf[u] = sx ^ dx;
}
}
ctx->ptr = ptr;
}




static void
do_final(br_eax_context *ctx)
{
size_t u;

do_pad(ctx);





for (u = 0; u < 16; u ++) {
ctx->cbcmac[u] ^= ctx->nonce[u] ^ ctx->head[u];
}
}


void
br_eax_get_tag(br_eax_context *ctx, void *tag)
{
do_final(ctx);
memcpy(tag, ctx->cbcmac, sizeof ctx->cbcmac);
}


void
br_eax_get_tag_trunc(br_eax_context *ctx, void *tag, size_t len)
{
do_final(ctx);
memcpy(tag, ctx->cbcmac, len);
}


uint32_t
br_eax_check_tag_trunc(br_eax_context *ctx, const void *tag, size_t len)
{
unsigned char tmp[16];
size_t u;
int x;

br_eax_get_tag(ctx, tmp);
x = 0;
for (u = 0; u < len; u ++) {
x |= tmp[u] ^ ((const unsigned char *)tag)[u];
}
return EQ0(x);
}


uint32_t
br_eax_check_tag(br_eax_context *ctx, const void *tag)
{
return br_eax_check_tag_trunc(ctx, tag, 16);
}


const br_aead_class br_eax_vtable = {
16,
(void (*)(const br_aead_class **, const void *, size_t))
&br_eax_reset,
(void (*)(const br_aead_class **, const void *, size_t))
&br_eax_aad_inject,
(void (*)(const br_aead_class **))
&br_eax_flip,
(void (*)(const br_aead_class **, int, void *, size_t))
&br_eax_run,
(void (*)(const br_aead_class **, void *))
&br_eax_get_tag,
(uint32_t (*)(const br_aead_class **, const void *))
&br_eax_check_tag,
(void (*)(const br_aead_class **, void *, size_t))
&br_eax_get_tag_trunc,
(uint32_t (*)(const br_aead_class **, const void *, size_t))
&br_eax_check_tag_trunc
};
