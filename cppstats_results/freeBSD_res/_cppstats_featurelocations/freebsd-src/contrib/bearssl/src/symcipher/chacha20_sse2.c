























#define BR_ENABLE_INTRINSICS 1
#include "inner.h"

#if BR_SSE2







br_chacha20_run
br_chacha20_sse2_get(void)
{







#if BR_amd64
return &br_chacha20_sse2_run;
#else




if (br_cpuid(0, 0, 0, 0x04000000)) {
return &br_chacha20_sse2_run;
} else {
return 0;
}
#endif
}

BR_TARGETS_X86_UP


BR_TARGET("sse2")
uint32_t
br_chacha20_sse2_run(const void *key,
const void *iv, uint32_t cc, void *data, size_t len)
{
unsigned char *buf;
uint32_t ivtmp[4];
__m128i kw0, kw1;
__m128i iw, cw;
__m128i one;

static const uint32_t CW[] = {
0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
};

buf = data;
kw0 = _mm_loadu_si128(key);
kw1 = _mm_loadu_si128((const void *)((const unsigned char *)key + 16));
ivtmp[0] = cc;
memcpy(ivtmp + 1, iv, 12);
iw = _mm_loadu_si128((const void *)ivtmp);
cw = _mm_loadu_si128((const void *)CW);
one = _mm_set_epi32(0, 0, 0, 1);

while (len > 0) {



__m128i s0, s1, s2, s3;
int i;

s0 = cw;
s1 = kw0;
s2 = kw1;
s3 = iw;
for (i = 0; i < 10; i ++) {




s0 = _mm_add_epi32(s0, s1);
s3 = _mm_xor_si128(s3, s0);
s3 = _mm_or_si128(
_mm_slli_epi32(s3, 16),
_mm_srli_epi32(s3, 16));

s2 = _mm_add_epi32(s2, s3);
s1 = _mm_xor_si128(s1, s2);
s1 = _mm_or_si128(
_mm_slli_epi32(s1, 12),
_mm_srli_epi32(s1, 20));

s0 = _mm_add_epi32(s0, s1);
s3 = _mm_xor_si128(s3, s0);
s3 = _mm_or_si128(
_mm_slli_epi32(s3, 8),
_mm_srli_epi32(s3, 24));

s2 = _mm_add_epi32(s2, s3);
s1 = _mm_xor_si128(s1, s2);
s1 = _mm_or_si128(
_mm_slli_epi32(s1, 7),
_mm_srli_epi32(s1, 25));






s1 = _mm_shuffle_epi32(s1, 0x39);
s2 = _mm_shuffle_epi32(s2, 0x4E);
s3 = _mm_shuffle_epi32(s3, 0x93);

s0 = _mm_add_epi32(s0, s1);
s3 = _mm_xor_si128(s3, s0);
s3 = _mm_or_si128(
_mm_slli_epi32(s3, 16),
_mm_srli_epi32(s3, 16));

s2 = _mm_add_epi32(s2, s3);
s1 = _mm_xor_si128(s1, s2);
s1 = _mm_or_si128(
_mm_slli_epi32(s1, 12),
_mm_srli_epi32(s1, 20));

s0 = _mm_add_epi32(s0, s1);
s3 = _mm_xor_si128(s3, s0);
s3 = _mm_or_si128(
_mm_slli_epi32(s3, 8),
_mm_srli_epi32(s3, 24));

s2 = _mm_add_epi32(s2, s3);
s1 = _mm_xor_si128(s1, s2);
s1 = _mm_or_si128(
_mm_slli_epi32(s1, 7),
_mm_srli_epi32(s1, 25));





s1 = _mm_shuffle_epi32(s1, 0x93);
s2 = _mm_shuffle_epi32(s2, 0x4E);
s3 = _mm_shuffle_epi32(s3, 0x39);
}




s0 = _mm_add_epi32(s0, cw);
s1 = _mm_add_epi32(s1, kw0);
s2 = _mm_add_epi32(s2, kw1);
s3 = _mm_add_epi32(s3, iw);




iw = _mm_add_epi32(iw, one);




if (len < 64) {
unsigned char tmp[64];
size_t u;

_mm_storeu_si128((void *)(tmp + 0), s0);
_mm_storeu_si128((void *)(tmp + 16), s1);
_mm_storeu_si128((void *)(tmp + 32), s2);
_mm_storeu_si128((void *)(tmp + 48), s3);
for (u = 0; u < len; u ++) {
buf[u] ^= tmp[u];
}
break;
} else {
__m128i b0, b1, b2, b3;

b0 = _mm_loadu_si128((const void *)(buf + 0));
b1 = _mm_loadu_si128((const void *)(buf + 16));
b2 = _mm_loadu_si128((const void *)(buf + 32));
b3 = _mm_loadu_si128((const void *)(buf + 48));
b0 = _mm_xor_si128(b0, s0);
b1 = _mm_xor_si128(b1, s1);
b2 = _mm_xor_si128(b2, s2);
b3 = _mm_xor_si128(b3, s3);
_mm_storeu_si128((void *)(buf + 0), b0);
_mm_storeu_si128((void *)(buf + 16), b1);
_mm_storeu_si128((void *)(buf + 32), b2);
_mm_storeu_si128((void *)(buf + 48), b3);
buf += 64;
len -= 64;
}
}





return (uint32_t)_mm_extract_epi16(iw, 0)
| ((uint32_t)_mm_extract_epi16(iw, 1) << 16);
}

BR_TARGETS_X86_DOWN

#else


br_chacha20_run
br_chacha20_sse2_get(void)
{
return 0;
}

#endif
