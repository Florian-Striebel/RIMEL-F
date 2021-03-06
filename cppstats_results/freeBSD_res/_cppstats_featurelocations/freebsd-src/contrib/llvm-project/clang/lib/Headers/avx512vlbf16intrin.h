







#if !defined(__IMMINTRIN_H)
#error "Never use <avx512vlbf16intrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__AVX512VLBF16INTRIN_H)
#define __AVX512VLBF16INTRIN_H

typedef short __m128bh __attribute__((__vector_size__(16), __aligned__(16)));

#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl, avx512bf16"), __min_vector_width__(128)))


#define __DEFAULT_FN_ATTRS256 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl, avx512bf16"), __min_vector_width__(256)))















static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_cvtne2ps_pbh(__m128 __A, __m128 __B) {
return (__m128bh)__builtin_ia32_cvtne2ps2bf16_128((__v4sf) __A,
(__v4sf) __B);
}


















static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_mask_cvtne2ps_pbh(__m128bh __W, __mmask8 __U, __m128 __A, __m128 __B) {
return (__m128bh)__builtin_ia32_selectw_128((__mmask8)__U,
(__v8hi)_mm_cvtne2ps_pbh(__A, __B),
(__v8hi)__W);
}
















static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_maskz_cvtne2ps_pbh(__mmask8 __U, __m128 __A, __m128 __B) {
return (__m128bh)__builtin_ia32_selectw_128((__mmask8)__U,
(__v8hi)_mm_cvtne2ps_pbh(__A, __B),
(__v8hi)_mm_setzero_si128());
}













static __inline__ __m256bh __DEFAULT_FN_ATTRS256
_mm256_cvtne2ps_pbh(__m256 __A, __m256 __B) {
return (__m256bh)__builtin_ia32_cvtne2ps2bf16_256((__v8sf) __A,
(__v8sf) __B);
}


















static __inline__ __m256bh __DEFAULT_FN_ATTRS256
_mm256_mask_cvtne2ps_pbh(__m256bh __W, __mmask16 __U, __m256 __A, __m256 __B) {
return (__m256bh)__builtin_ia32_selectw_256((__mmask16)__U,
(__v16hi)_mm256_cvtne2ps_pbh(__A, __B),
(__v16hi)__W);
}
















static __inline__ __m256bh __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtne2ps_pbh(__mmask16 __U, __m256 __A, __m256 __B) {
return (__m256bh)__builtin_ia32_selectw_256((__mmask16)__U,
(__v16hi)_mm256_cvtne2ps_pbh(__A, __B),
(__v16hi)_mm256_setzero_si256());
}











static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_cvtneps_pbh(__m128 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_128_mask((__v4sf) __A,
(__v8hi)_mm_undefined_si128(),
(__mmask8)-1);
}
















static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_mask_cvtneps_pbh(__m128bh __W, __mmask8 __U, __m128 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_128_mask((__v4sf) __A,
(__v8hi)__W,
(__mmask8)__U);
}














static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_maskz_cvtneps_pbh(__mmask8 __U, __m128 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_128_mask((__v4sf) __A,
(__v8hi)_mm_setzero_si128(),
(__mmask8)__U);
}










static __inline__ __m128bh __DEFAULT_FN_ATTRS256
_mm256_cvtneps_pbh(__m256 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_256_mask((__v8sf)__A,
(__v8hi)_mm_undefined_si128(),
(__mmask8)-1);
}















static __inline__ __m128bh __DEFAULT_FN_ATTRS256
_mm256_mask_cvtneps_pbh(__m128bh __W, __mmask8 __U, __m256 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_256_mask((__v8sf)__A,
(__v8hi)__W,
(__mmask8)__U);
}













static __inline__ __m128bh __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtneps_pbh(__mmask8 __U, __m256 __A) {
return (__m128bh)__builtin_ia32_cvtneps2bf16_256_mask((__v8sf)__A,
(__v8hi)_mm_setzero_si128(),
(__mmask8)__U);
}















static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_dpbf16_ps(__m128 __D, __m128bh __A, __m128bh __B) {
return (__m128)__builtin_ia32_dpbf16ps_128((__v4sf)__D,
(__v4si)__A,
(__v4si)__B);
}


















static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_dpbf16_ps(__m128 __D, __mmask8 __U, __m128bh __A, __m128bh __B) {
return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
(__v4sf)_mm_dpbf16_ps(__D, __A, __B),
(__v4sf)__D);
}


















static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_dpbf16_ps(__mmask8 __U, __m128 __D, __m128bh __A, __m128bh __B) {
return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
(__v4sf)_mm_dpbf16_ps(__D, __A, __B),
(__v4sf)_mm_setzero_si128());
}















static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_dpbf16_ps(__m256 __D, __m256bh __A, __m256bh __B) {
return (__m256)__builtin_ia32_dpbf16ps_256((__v8sf)__D,
(__v8si)__A,
(__v8si)__B);
}


















static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_dpbf16_ps(__m256 __D, __mmask8 __U, __m256bh __A, __m256bh __B) {
return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
(__v8sf)_mm256_dpbf16_ps(__D, __A, __B),
(__v8sf)__D);
}


















static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_dpbf16_ps(__mmask8 __U, __m256 __D, __m256bh __A, __m256bh __B) {
return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
(__v8sf)_mm256_dpbf16_ps(__D, __A, __B),
(__v8sf)_mm256_setzero_si256());
}











static __inline__ __bfloat16 __DEFAULT_FN_ATTRS128 _mm_cvtness_sbh(float __A) {
__v4sf __V = {__A, 0, 0, 0};
__v8hi __R = __builtin_ia32_cvtneps2bf16_128_mask(
(__v4sf)__V, (__v8hi)_mm_undefined_si128(), (__mmask8)-1);
return __R[0];
}








static __inline__ __m256 __DEFAULT_FN_ATTRS256 _mm256_cvtpbh_ps(__m128bh __A) {
return _mm256_castsi256_ps((__m256i)_mm256_slli_epi32(
(__m256i)_mm256_cvtepi16_epi32((__m128i)__A), 16));
}











static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtpbh_ps(__mmask8 __U, __m128bh __A) {
return _mm256_castsi256_ps((__m256i)_mm256_slli_epi32(
(__m256i)_mm256_maskz_cvtepi16_epi32((__mmask8)__U, (__m128i)__A), 16));
}














static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_cvtpbh_ps(__m256 __S, __mmask8 __U, __m128bh __A) {
return _mm256_castsi256_ps((__m256i)_mm256_mask_slli_epi32(
(__m256i)__S, (__mmask8)__U, (__m256i)_mm256_cvtepi16_epi32((__m128i)__A),
16));
}

#undef __DEFAULT_FN_ATTRS128
#undef __DEFAULT_FN_ATTRS256

#endif
