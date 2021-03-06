






















#if !defined(__IMMINTRIN_H)
#error "Never use <avx512vp2intersect.h> directly; include <immintrin.h> instead."
#endif

#if !defined(_AVX512VP2INTERSECT_H)
#define _AVX512VP2INTERSECT_H

#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("avx512vp2intersect"), __min_vector_width__(512)))


















static __inline__ void __DEFAULT_FN_ATTRS
_mm512_2intersect_epi32(__m512i __a, __m512i __b, __mmask16 *__m0, __mmask16 *__m1) {
__builtin_ia32_vp2intersect_d_512((__v16si)__a, (__v16si)__b, __m0, __m1);
}
















static __inline__ void __DEFAULT_FN_ATTRS
_mm512_2intersect_epi64(__m512i __a, __m512i __b, __mmask8 *__m0, __mmask8 *__m1) {
__builtin_ia32_vp2intersect_q_512((__v8di)__a, (__v8di)__b, __m0, __m1);
}

#undef __DEFAULT_FN_ATTRS

#endif
