























#define BR_ENABLE_INTRINSICS 1
#include "inner.h"






#if BR_AES_X86NI




static inline int
pclmul_supported(void)
{




return br_cpuid(0, 0, 0x00000002, 0);
}


br_ghash
br_ghash_pclmul_get(void)
{
return pclmul_supported() ? &br_ghash_pclmul : 0;
}

BR_TARGETS_X86_UP


































#if BR_CLANG && !BR_CLANG_3_8
#define BYTESWAP_DECL
#define BYTESWAP_PREP (void)0
#define BYTESWAP(x) do { __m128i byteswap1, byteswap2; byteswap1 = (x); byteswap2 = _mm_srli_epi16(byteswap1, 8); byteswap1 = _mm_slli_epi16(byteswap1, 8); byteswap1 = _mm_or_si128(byteswap1, byteswap2); byteswap1 = _mm_shufflelo_epi16(byteswap1, 0x1B); byteswap1 = _mm_shufflehi_epi16(byteswap1, 0x1B); (x) = _mm_shuffle_epi32(byteswap1, 0x4E); } while (0)









#else
#define BYTESWAP_DECL __m128i byteswap_index;
#define BYTESWAP_PREP do { byteswap_index = _mm_set_epi8( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); } while (0)



#define BYTESWAP(x) do { (x) = _mm_shuffle_epi8((x), byteswap_index); } while (0)


#endif











#if BR_CLANG
BR_TARGET("sse2")
static inline __m128i
pclmulqdq00(__m128i x, __m128i y)
{
__asm__ ("pclmulqdq $0x00, %1, %0" : "+x" (x) : "x" (y));
return x;
}
BR_TARGET("sse2")
static inline __m128i
pclmulqdq11(__m128i x, __m128i y)
{
__asm__ ("pclmulqdq $0x11, %1, %0" : "+x" (x) : "x" (y));
return x;
}
#else
#define pclmulqdq00(x, y) _mm_clmulepi64_si128(x, y, 0x00)
#define pclmulqdq11(x, y) _mm_clmulepi64_si128(x, y, 0x11)
#endif





#define BK(kw, kx) do { kx = _mm_xor_si128(kw, _mm_shuffle_epi32(kw, 0x0E)); } while (0)







#define PBK(k0, k1, kw, kx) do { kw = _mm_unpacklo_epi64(k1, k0); kx = _mm_xor_si128(k0, k1); } while (0)







#define SL_256(x0, x1, x2, x3) do { x0 = _mm_or_si128( _mm_slli_epi64(x0, 1), _mm_srli_epi64(x1, 63)); x1 = _mm_or_si128( _mm_slli_epi64(x1, 1), _mm_srli_epi64(x2, 63)); x2 = _mm_or_si128( _mm_slli_epi64(x2, 1), _mm_srli_epi64(x3, 63)); x3 = _mm_slli_epi64(x3, 1); } while (0)
















#define REDUCE_F128(x0, x1, x2, x3) do { x1 = _mm_xor_si128( x1, _mm_xor_si128( _mm_xor_si128( x3, _mm_srli_epi64(x3, 1)), _mm_xor_si128( _mm_srli_epi64(x3, 2), _mm_srli_epi64(x3, 7)))); x2 = _mm_xor_si128( _mm_xor_si128( x2, _mm_slli_epi64(x3, 63)), _mm_xor_si128( _mm_slli_epi64(x3, 62), _mm_slli_epi64(x3, 57))); x0 = _mm_xor_si128( x0, _mm_xor_si128( _mm_xor_si128( x2, _mm_srli_epi64(x2, 1)), _mm_xor_si128( _mm_srli_epi64(x2, 2), _mm_srli_epi64(x2, 7)))); x1 = _mm_xor_si128( _mm_xor_si128( x1, _mm_slli_epi64(x2, 63)), _mm_xor_si128( _mm_slli_epi64(x2, 62), _mm_slli_epi64(x2, 57))); } while (0)





































#define SQUARE_F128(kw, dw, dx) do { __m128i z0, z1, z2, z3; z1 = pclmulqdq11(kw, kw); z3 = pclmulqdq00(kw, kw); z0 = _mm_shuffle_epi32(z1, 0x0E); z2 = _mm_shuffle_epi32(z3, 0x0E); SL_256(z0, z1, z2, z3); REDUCE_F128(z0, z1, z2, z3); PBK(z0, z1, dw, dx); } while (0)











BR_TARGET("ssse3,pclmul")
void
br_ghash_pclmul(void *y, const void *h, const void *data, size_t len)
{
const unsigned char *buf1, *buf2;
unsigned char tmp[64];
size_t num4, num1;
__m128i yw, h1w, h1x;
BYTESWAP_DECL







buf1 = data;
num4 = len >> 6;
len &= 63;
buf2 = buf1 + (num4 << 6);
num1 = (len + 15) >> 4;
if ((len & 15) != 0) {
memcpy(tmp, buf2, len);
memset(tmp + len, 0, (num1 << 4) - len);
buf2 = tmp;
}




BYTESWAP_PREP;




yw = _mm_loadu_si128(y);
h1w = _mm_loadu_si128(h);
BYTESWAP(yw);
BYTESWAP(h1w);
BK(h1w, h1x);

if (num4 > 0) {
__m128i h2w, h2x, h3w, h3x, h4w, h4x;
__m128i t0, t1, t2, t3;




SQUARE_F128(h1w, h2w, h2x);




t1 = pclmulqdq11(h1w, h2w);
t3 = pclmulqdq00(h1w, h2w);
t2 = _mm_xor_si128(pclmulqdq00(h1x, h2x),
_mm_xor_si128(t1, t3));
t0 = _mm_shuffle_epi32(t1, 0x0E);
t1 = _mm_xor_si128(t1, _mm_shuffle_epi32(t2, 0x0E));
t2 = _mm_xor_si128(t2, _mm_shuffle_epi32(t3, 0x0E));
SL_256(t0, t1, t2, t3);
REDUCE_F128(t0, t1, t2, t3);
PBK(t0, t1, h3w, h3x);




SQUARE_F128(h2w, h4w, h4x);

while (num4 -- > 0) {
__m128i aw0, aw1, aw2, aw3;
__m128i ax0, ax1, ax2, ax3;

aw0 = _mm_loadu_si128((void *)(buf1 + 0));
aw1 = _mm_loadu_si128((void *)(buf1 + 16));
aw2 = _mm_loadu_si128((void *)(buf1 + 32));
aw3 = _mm_loadu_si128((void *)(buf1 + 48));
BYTESWAP(aw0);
BYTESWAP(aw1);
BYTESWAP(aw2);
BYTESWAP(aw3);
buf1 += 64;

aw0 = _mm_xor_si128(aw0, yw);
BK(aw1, ax1);
BK(aw2, ax2);
BK(aw3, ax3);
BK(aw0, ax0);

t1 = _mm_xor_si128(
_mm_xor_si128(
pclmulqdq11(aw0, h4w),
pclmulqdq11(aw1, h3w)),
_mm_xor_si128(
pclmulqdq11(aw2, h2w),
pclmulqdq11(aw3, h1w)));
t3 = _mm_xor_si128(
_mm_xor_si128(
pclmulqdq00(aw0, h4w),
pclmulqdq00(aw1, h3w)),
_mm_xor_si128(
pclmulqdq00(aw2, h2w),
pclmulqdq00(aw3, h1w)));
t2 = _mm_xor_si128(
_mm_xor_si128(
pclmulqdq00(ax0, h4x),
pclmulqdq00(ax1, h3x)),
_mm_xor_si128(
pclmulqdq00(ax2, h2x),
pclmulqdq00(ax3, h1x)));
t2 = _mm_xor_si128(t2, _mm_xor_si128(t1, t3));
t0 = _mm_shuffle_epi32(t1, 0x0E);
t1 = _mm_xor_si128(t1, _mm_shuffle_epi32(t2, 0x0E));
t2 = _mm_xor_si128(t2, _mm_shuffle_epi32(t3, 0x0E));
SL_256(t0, t1, t2, t3);
REDUCE_F128(t0, t1, t2, t3);
yw = _mm_unpacklo_epi64(t1, t0);
}
}

while (num1 -- > 0) {
__m128i aw, ax;
__m128i t0, t1, t2, t3;

aw = _mm_loadu_si128((void *)buf2);
BYTESWAP(aw);
buf2 += 16;

aw = _mm_xor_si128(aw, yw);
BK(aw, ax);

t1 = pclmulqdq11(aw, h1w);
t3 = pclmulqdq00(aw, h1w);
t2 = pclmulqdq00(ax, h1x);
t2 = _mm_xor_si128(t2, _mm_xor_si128(t1, t3));
t0 = _mm_shuffle_epi32(t1, 0x0E);
t1 = _mm_xor_si128(t1, _mm_shuffle_epi32(t2, 0x0E));
t2 = _mm_xor_si128(t2, _mm_shuffle_epi32(t3, 0x0E));
SL_256(t0, t1, t2, t3);
REDUCE_F128(t0, t1, t2, t3);
yw = _mm_unpacklo_epi64(t1, t0);
}

BYTESWAP(yw);
_mm_storeu_si128(y, yw);
}

BR_TARGETS_X86_DOWN

#else


br_ghash
br_ghash_pclmul_get(void)
{
return 0;
}

#endif
