






















#if !defined(__IMMINTRIN_H)
#error "Never use <avx512vlvp2intersectintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(_AVX512VLVP2INTERSECT_H)
#define _AVX512VLVP2INTERSECT_H

#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl,avx512vp2intersect"), __min_vector_width__(128)))



#define __DEFAULT_FN_ATTRS256 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl,avx512vp2intersect"), __min_vector_width__(256)))

















static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_2intersect_epi32(__m256i __a, __m256i __b, __mmask8 *__m0, __mmask8 *__m1) {
__builtin_ia32_vp2intersect_d_256((__v8si)__a, (__v8si)__b, __m0, __m1);
}
















static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_2intersect_epi64(__m256i __a, __m256i __b, __mmask8 *__m0, __mmask8 *__m1) {
__builtin_ia32_vp2intersect_q_256((__v4di)__a, (__v4di)__b, __m0, __m1);
}
















static __inline__ void __DEFAULT_FN_ATTRS128
_mm_2intersect_epi32(__m128i __a, __m128i __b, __mmask8 *__m0, __mmask8 *__m1) {
__builtin_ia32_vp2intersect_d_128((__v4si)__a, (__v4si)__b, __m0, __m1);
}
















static __inline__ void __DEFAULT_FN_ATTRS128
_mm_2intersect_epi64(__m128i __a, __m128i __b, __mmask8 *__m0, __mmask8 *__m1) {
__builtin_ia32_vp2intersect_q_128((__v2di)__a, (__v2di)__b, __m0, __m1);
}

#undef __DEFAULT_FN_ATTRS128
#undef __DEFAULT_FN_ATTRS256

#endif
