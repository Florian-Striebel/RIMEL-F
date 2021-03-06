







#if !defined(__IMMINTRIN_H)
#error "Never use <avx512bf16intrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__AVX512BF16INTRIN_H)
#define __AVX512BF16INTRIN_H

typedef short __m512bh __attribute__((__vector_size__(64), __aligned__(64)));
typedef short __m256bh __attribute__((__vector_size__(32), __aligned__(32)));
typedef unsigned short __bfloat16;

#define __DEFAULT_FN_ATTRS512 __attribute__((__always_inline__, __nodebug__, __target__("avx512bf16"), __min_vector_width__(512)))


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("avx512bf16")))












static __inline__ float __DEFAULT_FN_ATTRS _mm_cvtsbh_ss(__bfloat16 __A) {
return __builtin_ia32_cvtsbf162ss_32(__A);
}













static __inline__ __m512bh __DEFAULT_FN_ATTRS512
_mm512_cvtne2ps_pbh(__m512 __A, __m512 __B) {
return (__m512bh)__builtin_ia32_cvtne2ps2bf16_512((__v16sf) __A,
(__v16sf) __B);
}


















static __inline__ __m512bh __DEFAULT_FN_ATTRS512
_mm512_mask_cvtne2ps_pbh(__m512bh __W, __mmask32 __U, __m512 __A, __m512 __B) {
return (__m512bh)__builtin_ia32_selectw_512((__mmask32)__U,
(__v32hi)_mm512_cvtne2ps_pbh(__A, __B),
(__v32hi)__W);
}
















static __inline__ __m512bh __DEFAULT_FN_ATTRS512
_mm512_maskz_cvtne2ps_pbh(__mmask32 __U, __m512 __A, __m512 __B) {
return (__m512bh)__builtin_ia32_selectw_512((__mmask32)__U,
(__v32hi)_mm512_cvtne2ps_pbh(__A, __B),
(__v32hi)_mm512_setzero_si512());
}










static __inline__ __m256bh __DEFAULT_FN_ATTRS512
_mm512_cvtneps_pbh(__m512 __A) {
return (__m256bh)__builtin_ia32_cvtneps2bf16_512_mask((__v16sf)__A,
(__v16hi)_mm256_undefined_si256(),
(__mmask16)-1);
}















static __inline__ __m256bh __DEFAULT_FN_ATTRS512
_mm512_mask_cvtneps_pbh(__m256bh __W, __mmask16 __U, __m512 __A) {
return (__m256bh)__builtin_ia32_cvtneps2bf16_512_mask((__v16sf)__A,
(__v16hi)__W,
(__mmask16)__U);
}













static __inline__ __m256bh __DEFAULT_FN_ATTRS512
_mm512_maskz_cvtneps_pbh(__mmask16 __U, __m512 __A) {
return (__m256bh)__builtin_ia32_cvtneps2bf16_512_mask((__v16sf)__A,
(__v16hi)_mm256_setzero_si256(),
(__mmask16)__U);
}















static __inline__ __m512 __DEFAULT_FN_ATTRS512
_mm512_dpbf16_ps(__m512 __D, __m512bh __A, __m512bh __B) {
return (__m512)__builtin_ia32_dpbf16ps_512((__v16sf) __D,
(__v16si) __A,
(__v16si) __B);
}


















static __inline__ __m512 __DEFAULT_FN_ATTRS512
_mm512_mask_dpbf16_ps(__m512 __D, __mmask16 __U, __m512bh __A, __m512bh __B) {
return (__m512)__builtin_ia32_selectps_512((__mmask16)__U,
(__v16sf)_mm512_dpbf16_ps(__D, __A, __B),
(__v16sf)__D);
}


















static __inline__ __m512 __DEFAULT_FN_ATTRS512
_mm512_maskz_dpbf16_ps(__mmask16 __U, __m512 __D, __m512bh __A, __m512bh __B) {
return (__m512)__builtin_ia32_selectps_512((__mmask16)__U,
(__v16sf)_mm512_dpbf16_ps(__D, __A, __B),
(__v16sf)_mm512_setzero_si512());
}








static __inline__ __m512 __DEFAULT_FN_ATTRS512 _mm512_cvtpbh_ps(__m256bh __A) {
return _mm512_castsi512_ps((__m512i)_mm512_slli_epi32(
(__m512i)_mm512_cvtepi16_epi32((__m256i)__A), 16));
}











static __inline__ __m512 __DEFAULT_FN_ATTRS512
_mm512_maskz_cvtpbh_ps(__mmask16 __U, __m256bh __A) {
return _mm512_castsi512_ps((__m512i)_mm512_slli_epi32(
(__m512i)_mm512_maskz_cvtepi16_epi32((__mmask16)__U, (__m256i)__A), 16));
}













static __inline__ __m512 __DEFAULT_FN_ATTRS512
_mm512_mask_cvtpbh_ps(__m512 __S, __mmask16 __U, __m256bh __A) {
return _mm512_castsi512_ps((__m512i)_mm512_mask_slli_epi32(
(__m512i)__S, (__mmask16)__U,
(__m512i)_mm512_cvtepi16_epi32((__m256i)__A), 16));
}

#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS512

#endif
