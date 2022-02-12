








#if !defined(__WMMINTRIN_H)
#error "Never use <__wmmintrin_aes.h> directly; include <wmmintrin.h> instead."
#endif

#if !defined(__WMMINTRIN_AES_H)
#define __WMMINTRIN_AES_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("aes"), __min_vector_width__(128)))















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_aesenc_si128(__m128i __V, __m128i __R)
{
return (__m128i)__builtin_ia32_aesenc128((__v2di)__V, (__v2di)__R);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_aesenclast_si128(__m128i __V, __m128i __R)
{
return (__m128i)__builtin_ia32_aesenclast128((__v2di)__V, (__v2di)__R);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_aesdec_si128(__m128i __V, __m128i __R)
{
return (__m128i)__builtin_ia32_aesdec128((__v2di)__V, (__v2di)__R);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_aesdeclast_si128(__m128i __V, __m128i __R)
{
return (__m128i)__builtin_ia32_aesdeclast128((__v2di)__V, (__v2di)__R);
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_aesimc_si128(__m128i __V)
{
return (__m128i)__builtin_ia32_aesimc128((__v2di)__V);
}



















#define _mm_aeskeygenassist_si128(C, R) (__m128i)__builtin_ia32_aeskeygenassist128((__v2di)(__m128i)(C), (int)(R))


#undef __DEFAULT_FN_ATTRS

#endif
