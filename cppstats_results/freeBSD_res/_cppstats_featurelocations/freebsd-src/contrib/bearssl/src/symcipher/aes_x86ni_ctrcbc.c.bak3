























#define BR_ENABLE_INTRINSICS 1
#include "inner.h"

#if BR_AES_X86NI


const br_block_ctrcbc_class *
br_aes_x86ni_ctrcbc_get_vtable(void)
{
return br_aes_x86ni_supported() ? &br_aes_x86ni_ctrcbc_vtable : NULL;
}


void
br_aes_x86ni_ctrcbc_init(br_aes_x86ni_ctrcbc_keys *ctx,
const void *key, size_t len)
{
ctx->vtable = &br_aes_x86ni_ctrcbc_vtable;
ctx->num_rounds = br_aes_x86ni_keysched_enc(ctx->skey.skni, key, len);
}

BR_TARGETS_X86_UP


BR_TARGET("sse2,sse4.1,aes")
void
br_aes_x86ni_ctrcbc_ctr(const br_aes_x86ni_ctrcbc_keys *ctx,
void *ctr, void *data, size_t len)
{
unsigned char *buf;
unsigned num_rounds;
__m128i sk[15];
__m128i ivx0, ivx1, ivx2, ivx3;
__m128i erev, zero, one, four, notthree;
unsigned u;

buf = data;
num_rounds = ctx->num_rounds;
for (u = 0; u <= num_rounds; u ++) {
sk[u] = _mm_loadu_si128((void *)(ctx->skey.skni + (u << 4)));
}




erev = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15);
zero = _mm_setzero_si128();
one = _mm_set_epi64x(0, 1);
four = _mm_set_epi64x(0, 4);
notthree = _mm_sub_epi64(zero, four);





ivx0 = _mm_shuffle_epi8(_mm_loadu_si128((void *)ctr), erev);
ivx1 = _mm_add_epi64(ivx0, one);
ivx1 = _mm_sub_epi64(ivx1,
_mm_slli_si128(_mm_cmpeq_epi64(ivx1, zero), 8));
ivx2 = _mm_add_epi64(ivx1, one);
ivx2 = _mm_sub_epi64(ivx2,
_mm_slli_si128(_mm_cmpeq_epi64(ivx2, zero), 8));
ivx3 = _mm_add_epi64(ivx2, one);
ivx3 = _mm_sub_epi64(ivx3,
_mm_slli_si128(_mm_cmpeq_epi64(ivx3, zero), 8));
while (len > 0) {
__m128i x0, x1, x2, x3;





x0 = _mm_shuffle_epi8(ivx0, erev);
x1 = _mm_shuffle_epi8(ivx1, erev);
x2 = _mm_shuffle_epi8(ivx2, erev);
x3 = _mm_shuffle_epi8(ivx3, erev);

x0 = _mm_xor_si128(x0, sk[0]);
x1 = _mm_xor_si128(x1, sk[0]);
x2 = _mm_xor_si128(x2, sk[0]);
x3 = _mm_xor_si128(x3, sk[0]);
x0 = _mm_aesenc_si128(x0, sk[1]);
x1 = _mm_aesenc_si128(x1, sk[1]);
x2 = _mm_aesenc_si128(x2, sk[1]);
x3 = _mm_aesenc_si128(x3, sk[1]);
x0 = _mm_aesenc_si128(x0, sk[2]);
x1 = _mm_aesenc_si128(x1, sk[2]);
x2 = _mm_aesenc_si128(x2, sk[2]);
x3 = _mm_aesenc_si128(x3, sk[2]);
x0 = _mm_aesenc_si128(x0, sk[3]);
x1 = _mm_aesenc_si128(x1, sk[3]);
x2 = _mm_aesenc_si128(x2, sk[3]);
x3 = _mm_aesenc_si128(x3, sk[3]);
x0 = _mm_aesenc_si128(x0, sk[4]);
x1 = _mm_aesenc_si128(x1, sk[4]);
x2 = _mm_aesenc_si128(x2, sk[4]);
x3 = _mm_aesenc_si128(x3, sk[4]);
x0 = _mm_aesenc_si128(x0, sk[5]);
x1 = _mm_aesenc_si128(x1, sk[5]);
x2 = _mm_aesenc_si128(x2, sk[5]);
x3 = _mm_aesenc_si128(x3, sk[5]);
x0 = _mm_aesenc_si128(x0, sk[6]);
x1 = _mm_aesenc_si128(x1, sk[6]);
x2 = _mm_aesenc_si128(x2, sk[6]);
x3 = _mm_aesenc_si128(x3, sk[6]);
x0 = _mm_aesenc_si128(x0, sk[7]);
x1 = _mm_aesenc_si128(x1, sk[7]);
x2 = _mm_aesenc_si128(x2, sk[7]);
x3 = _mm_aesenc_si128(x3, sk[7]);
x0 = _mm_aesenc_si128(x0, sk[8]);
x1 = _mm_aesenc_si128(x1, sk[8]);
x2 = _mm_aesenc_si128(x2, sk[8]);
x3 = _mm_aesenc_si128(x3, sk[8]);
x0 = _mm_aesenc_si128(x0, sk[9]);
x1 = _mm_aesenc_si128(x1, sk[9]);
x2 = _mm_aesenc_si128(x2, sk[9]);
x3 = _mm_aesenc_si128(x3, sk[9]);
if (num_rounds == 10) {
x0 = _mm_aesenclast_si128(x0, sk[10]);
x1 = _mm_aesenclast_si128(x1, sk[10]);
x2 = _mm_aesenclast_si128(x2, sk[10]);
x3 = _mm_aesenclast_si128(x3, sk[10]);
} else if (num_rounds == 12) {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x2 = _mm_aesenc_si128(x2, sk[10]);
x3 = _mm_aesenc_si128(x3, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x2 = _mm_aesenc_si128(x2, sk[11]);
x3 = _mm_aesenc_si128(x3, sk[11]);
x0 = _mm_aesenclast_si128(x0, sk[12]);
x1 = _mm_aesenclast_si128(x1, sk[12]);
x2 = _mm_aesenclast_si128(x2, sk[12]);
x3 = _mm_aesenclast_si128(x3, sk[12]);
} else {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x2 = _mm_aesenc_si128(x2, sk[10]);
x3 = _mm_aesenc_si128(x3, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x2 = _mm_aesenc_si128(x2, sk[11]);
x3 = _mm_aesenc_si128(x3, sk[11]);
x0 = _mm_aesenc_si128(x0, sk[12]);
x1 = _mm_aesenc_si128(x1, sk[12]);
x2 = _mm_aesenc_si128(x2, sk[12]);
x3 = _mm_aesenc_si128(x3, sk[12]);
x0 = _mm_aesenc_si128(x0, sk[13]);
x1 = _mm_aesenc_si128(x1, sk[13]);
x2 = _mm_aesenc_si128(x2, sk[13]);
x3 = _mm_aesenc_si128(x3, sk[13]);
x0 = _mm_aesenclast_si128(x0, sk[14]);
x1 = _mm_aesenclast_si128(x1, sk[14]);
x2 = _mm_aesenclast_si128(x2, sk[14]);
x3 = _mm_aesenclast_si128(x3, sk[14]);
}
if (len >= 64) {
x0 = _mm_xor_si128(x0,
_mm_loadu_si128((void *)(buf + 0)));
x1 = _mm_xor_si128(x1,
_mm_loadu_si128((void *)(buf + 16)));
x2 = _mm_xor_si128(x2,
_mm_loadu_si128((void *)(buf + 32)));
x3 = _mm_xor_si128(x3,
_mm_loadu_si128((void *)(buf + 48)));
_mm_storeu_si128((void *)(buf + 0), x0);
_mm_storeu_si128((void *)(buf + 16), x1);
_mm_storeu_si128((void *)(buf + 32), x2);
_mm_storeu_si128((void *)(buf + 48), x3);
buf += 64;
len -= 64;
} else {
unsigned char tmp[64];

_mm_storeu_si128((void *)(tmp + 0), x0);
_mm_storeu_si128((void *)(tmp + 16), x1);
_mm_storeu_si128((void *)(tmp + 32), x2);
_mm_storeu_si128((void *)(tmp + 48), x3);
for (u = 0; u < len; u ++) {
buf[u] ^= tmp[u];
}
switch (len) {
case 16:
ivx0 = ivx1;
break;
case 32:
ivx0 = ivx2;
break;
case 48:
ivx0 = ivx3;
break;
}
break;
}








ivx0 = _mm_add_epi64(ivx0, four);
ivx1 = _mm_add_epi64(ivx1, four);
ivx2 = _mm_add_epi64(ivx2, four);
ivx3 = _mm_add_epi64(ivx3, four);
ivx0 = _mm_sub_epi64(ivx0,
_mm_slli_si128(_mm_cmpeq_epi64(
_mm_and_si128(ivx0, notthree), zero), 8));
ivx1 = _mm_sub_epi64(ivx1,
_mm_slli_si128(_mm_cmpeq_epi64(
_mm_and_si128(ivx1, notthree), zero), 8));
ivx2 = _mm_sub_epi64(ivx2,
_mm_slli_si128(_mm_cmpeq_epi64(
_mm_and_si128(ivx2, notthree), zero), 8));
ivx3 = _mm_sub_epi64(ivx3,
_mm_slli_si128(_mm_cmpeq_epi64(
_mm_and_si128(ivx3, notthree), zero), 8));
}





_mm_storeu_si128((void *)ctr, _mm_shuffle_epi8(ivx0, erev));
}


BR_TARGET("sse2,sse4.1,aes")
void
br_aes_x86ni_ctrcbc_mac(const br_aes_x86ni_ctrcbc_keys *ctx,
void *cbcmac, const void *data, size_t len)
{
const unsigned char *buf;
unsigned num_rounds;
__m128i sk[15], ivx;
unsigned u;

buf = data;
ivx = _mm_loadu_si128(cbcmac);
num_rounds = ctx->num_rounds;
for (u = 0; u <= num_rounds; u ++) {
sk[u] = _mm_loadu_si128((void *)(ctx->skey.skni + (u << 4)));
}
while (len > 0) {
__m128i x;

x = _mm_xor_si128(_mm_loadu_si128((void *)buf), ivx);
x = _mm_xor_si128(x, sk[0]);
x = _mm_aesenc_si128(x, sk[1]);
x = _mm_aesenc_si128(x, sk[2]);
x = _mm_aesenc_si128(x, sk[3]);
x = _mm_aesenc_si128(x, sk[4]);
x = _mm_aesenc_si128(x, sk[5]);
x = _mm_aesenc_si128(x, sk[6]);
x = _mm_aesenc_si128(x, sk[7]);
x = _mm_aesenc_si128(x, sk[8]);
x = _mm_aesenc_si128(x, sk[9]);
if (num_rounds == 10) {
x = _mm_aesenclast_si128(x, sk[10]);
} else if (num_rounds == 12) {
x = _mm_aesenc_si128(x, sk[10]);
x = _mm_aesenc_si128(x, sk[11]);
x = _mm_aesenclast_si128(x, sk[12]);
} else {
x = _mm_aesenc_si128(x, sk[10]);
x = _mm_aesenc_si128(x, sk[11]);
x = _mm_aesenc_si128(x, sk[12]);
x = _mm_aesenc_si128(x, sk[13]);
x = _mm_aesenclast_si128(x, sk[14]);
}
ivx = x;
buf += 16;
len -= 16;
}
_mm_storeu_si128(cbcmac, ivx);
}


BR_TARGET("sse2,sse4.1,aes")
void
br_aes_x86ni_ctrcbc_encrypt(const br_aes_x86ni_ctrcbc_keys *ctx,
void *ctr, void *cbcmac, void *data, size_t len)
{
unsigned char *buf;
unsigned num_rounds;
__m128i sk[15];
__m128i ivx, cmx;
__m128i erev, zero, one;
unsigned u;
int first_iter;

num_rounds = ctx->num_rounds;
for (u = 0; u <= num_rounds; u ++) {
sk[u] = _mm_loadu_si128((void *)(ctx->skey.skni + (u << 4)));
}




erev = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15);
zero = _mm_setzero_si128();
one = _mm_set_epi64x(0, 1);




ivx = _mm_shuffle_epi8(_mm_loadu_si128(ctr), erev);
cmx = _mm_loadu_si128(cbcmac);

buf = data;
first_iter = 1;
while (len > 0) {
__m128i dx, x0, x1;







dx = _mm_loadu_si128((void *)buf);
x0 = _mm_shuffle_epi8(ivx, erev);
x1 = cmx;

x0 = _mm_xor_si128(x0, sk[0]);
x1 = _mm_xor_si128(x1, sk[0]);
x0 = _mm_aesenc_si128(x0, sk[1]);
x1 = _mm_aesenc_si128(x1, sk[1]);
x0 = _mm_aesenc_si128(x0, sk[2]);
x1 = _mm_aesenc_si128(x1, sk[2]);
x0 = _mm_aesenc_si128(x0, sk[3]);
x1 = _mm_aesenc_si128(x1, sk[3]);
x0 = _mm_aesenc_si128(x0, sk[4]);
x1 = _mm_aesenc_si128(x1, sk[4]);
x0 = _mm_aesenc_si128(x0, sk[5]);
x1 = _mm_aesenc_si128(x1, sk[5]);
x0 = _mm_aesenc_si128(x0, sk[6]);
x1 = _mm_aesenc_si128(x1, sk[6]);
x0 = _mm_aesenc_si128(x0, sk[7]);
x1 = _mm_aesenc_si128(x1, sk[7]);
x0 = _mm_aesenc_si128(x0, sk[8]);
x1 = _mm_aesenc_si128(x1, sk[8]);
x0 = _mm_aesenc_si128(x0, sk[9]);
x1 = _mm_aesenc_si128(x1, sk[9]);
if (num_rounds == 10) {
x0 = _mm_aesenclast_si128(x0, sk[10]);
x1 = _mm_aesenclast_si128(x1, sk[10]);
} else if (num_rounds == 12) {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x0 = _mm_aesenclast_si128(x0, sk[12]);
x1 = _mm_aesenclast_si128(x1, sk[12]);
} else {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x0 = _mm_aesenc_si128(x0, sk[12]);
x1 = _mm_aesenc_si128(x1, sk[12]);
x0 = _mm_aesenc_si128(x0, sk[13]);
x1 = _mm_aesenc_si128(x1, sk[13]);
x0 = _mm_aesenclast_si128(x0, sk[14]);
x1 = _mm_aesenclast_si128(x1, sk[14]);
}

x0 = _mm_xor_si128(x0, dx);
if (first_iter) {
cmx = _mm_xor_si128(cmx, x0);
first_iter = 0;
} else {
cmx = _mm_xor_si128(x1, x0);
}
_mm_storeu_si128((void *)buf, x0);

buf += 16;
len -= 16;




ivx = _mm_add_epi64(ivx, one);
ivx = _mm_sub_epi64(ivx,
_mm_slli_si128(_mm_cmpeq_epi64(ivx, zero), 8));





if (len == 0) {
cmx = _mm_xor_si128(cmx, sk[0]);
cmx = _mm_aesenc_si128(cmx, sk[1]);
cmx = _mm_aesenc_si128(cmx, sk[2]);
cmx = _mm_aesenc_si128(cmx, sk[3]);
cmx = _mm_aesenc_si128(cmx, sk[4]);
cmx = _mm_aesenc_si128(cmx, sk[5]);
cmx = _mm_aesenc_si128(cmx, sk[6]);
cmx = _mm_aesenc_si128(cmx, sk[7]);
cmx = _mm_aesenc_si128(cmx, sk[8]);
cmx = _mm_aesenc_si128(cmx, sk[9]);
if (num_rounds == 10) {
cmx = _mm_aesenclast_si128(cmx, sk[10]);
} else if (num_rounds == 12) {
cmx = _mm_aesenc_si128(cmx, sk[10]);
cmx = _mm_aesenc_si128(cmx, sk[11]);
cmx = _mm_aesenclast_si128(cmx, sk[12]);
} else {
cmx = _mm_aesenc_si128(cmx, sk[10]);
cmx = _mm_aesenc_si128(cmx, sk[11]);
cmx = _mm_aesenc_si128(cmx, sk[12]);
cmx = _mm_aesenc_si128(cmx, sk[13]);
cmx = _mm_aesenclast_si128(cmx, sk[14]);
}
break;
}
}




_mm_storeu_si128(ctr, _mm_shuffle_epi8(ivx, erev));
_mm_storeu_si128(cbcmac, cmx);
}


BR_TARGET("sse2,sse4.1,aes")
void
br_aes_x86ni_ctrcbc_decrypt(const br_aes_x86ni_ctrcbc_keys *ctx,
void *ctr, void *cbcmac, void *data, size_t len)
{
unsigned char *buf;
unsigned num_rounds;
__m128i sk[15];
__m128i ivx, cmx;
__m128i erev, zero, one;
unsigned u;

num_rounds = ctx->num_rounds;
for (u = 0; u <= num_rounds; u ++) {
sk[u] = _mm_loadu_si128((void *)(ctx->skey.skni + (u << 4)));
}




erev = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15);
zero = _mm_setzero_si128();
one = _mm_set_epi64x(0, 1);




ivx = _mm_shuffle_epi8(_mm_loadu_si128(ctr), erev);
cmx = _mm_loadu_si128(cbcmac);

buf = data;
while (len > 0) {
__m128i dx, x0, x1;







dx = _mm_loadu_si128((void *)buf);
x0 = _mm_shuffle_epi8(ivx, erev);
x1 = _mm_xor_si128(cmx, dx);

x0 = _mm_xor_si128(x0, sk[0]);
x1 = _mm_xor_si128(x1, sk[0]);
x0 = _mm_aesenc_si128(x0, sk[1]);
x1 = _mm_aesenc_si128(x1, sk[1]);
x0 = _mm_aesenc_si128(x0, sk[2]);
x1 = _mm_aesenc_si128(x1, sk[2]);
x0 = _mm_aesenc_si128(x0, sk[3]);
x1 = _mm_aesenc_si128(x1, sk[3]);
x0 = _mm_aesenc_si128(x0, sk[4]);
x1 = _mm_aesenc_si128(x1, sk[4]);
x0 = _mm_aesenc_si128(x0, sk[5]);
x1 = _mm_aesenc_si128(x1, sk[5]);
x0 = _mm_aesenc_si128(x0, sk[6]);
x1 = _mm_aesenc_si128(x1, sk[6]);
x0 = _mm_aesenc_si128(x0, sk[7]);
x1 = _mm_aesenc_si128(x1, sk[7]);
x0 = _mm_aesenc_si128(x0, sk[8]);
x1 = _mm_aesenc_si128(x1, sk[8]);
x0 = _mm_aesenc_si128(x0, sk[9]);
x1 = _mm_aesenc_si128(x1, sk[9]);
if (num_rounds == 10) {
x0 = _mm_aesenclast_si128(x0, sk[10]);
x1 = _mm_aesenclast_si128(x1, sk[10]);
} else if (num_rounds == 12) {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x0 = _mm_aesenclast_si128(x0, sk[12]);
x1 = _mm_aesenclast_si128(x1, sk[12]);
} else {
x0 = _mm_aesenc_si128(x0, sk[10]);
x1 = _mm_aesenc_si128(x1, sk[10]);
x0 = _mm_aesenc_si128(x0, sk[11]);
x1 = _mm_aesenc_si128(x1, sk[11]);
x0 = _mm_aesenc_si128(x0, sk[12]);
x1 = _mm_aesenc_si128(x1, sk[12]);
x0 = _mm_aesenc_si128(x0, sk[13]);
x1 = _mm_aesenc_si128(x1, sk[13]);
x0 = _mm_aesenclast_si128(x0, sk[14]);
x1 = _mm_aesenclast_si128(x1, sk[14]);
}
x0 = _mm_xor_si128(x0, dx);
cmx = x1;
_mm_storeu_si128((void *)buf, x0);

buf += 16;
len -= 16;




ivx = _mm_add_epi64(ivx, one);
ivx = _mm_sub_epi64(ivx,
_mm_slli_si128(_mm_cmpeq_epi64(ivx, zero), 8));
}




_mm_storeu_si128(ctr, _mm_shuffle_epi8(ivx, erev));
_mm_storeu_si128(cbcmac, cmx);
}

BR_TARGETS_X86_DOWN


const br_block_ctrcbc_class br_aes_x86ni_ctrcbc_vtable = {
sizeof(br_aes_x86ni_ctrcbc_keys),
16,
4,
(void (*)(const br_block_ctrcbc_class **, const void *, size_t))
&br_aes_x86ni_ctrcbc_init,
(void (*)(const br_block_ctrcbc_class *const *,
void *, void *, void *, size_t))
&br_aes_x86ni_ctrcbc_encrypt,
(void (*)(const br_block_ctrcbc_class *const *,
void *, void *, void *, size_t))
&br_aes_x86ni_ctrcbc_decrypt,
(void (*)(const br_block_ctrcbc_class *const *,
void *, void *, size_t))
&br_aes_x86ni_ctrcbc_ctr,
(void (*)(const br_block_ctrcbc_class *const *,
void *, const void *, size_t))
&br_aes_x86ni_ctrcbc_mac
};

#else


const br_block_ctrcbc_class *
br_aes_x86ni_ctrcbc_get_vtable(void)
{
return NULL;
}

#endif
