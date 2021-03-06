






















#if !defined(__IMMINTRIN_H)
#error "Never use <avxvnniintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__AVXVNNIINTRIN_H)
#define __AVXVNNIINTRIN_H













#define __DEFAULT_FN_ATTRS256 __attribute__((__always_inline__, __nodebug__, __target__("avxvnni"), __min_vector_width__(256)))
#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avxvnni"), __min_vector_width__(128)))


















static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_dpbusd_avx_epi32(__m256i __S, __m256i __A, __m256i __B)
{
return (__m256i)__builtin_ia32_vpdpbusd256((__v8si)__S, (__v8si)__A, (__v8si)__B);
}


















static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_dpbusds_avx_epi32(__m256i __S, __m256i __A, __m256i __B)
{
return (__m256i)__builtin_ia32_vpdpbusds256((__v8si)__S, (__v8si)__A, (__v8si)__B);
}
















static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_dpwssd_avx_epi32(__m256i __S, __m256i __A, __m256i __B)
{
return (__m256i)__builtin_ia32_vpdpwssd256((__v8si)__S, (__v8si)__A, (__v8si)__B);
}
















static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_dpwssds_avx_epi32(__m256i __S, __m256i __A, __m256i __B)
{
return (__m256i)__builtin_ia32_vpdpwssds256((__v8si)__S, (__v8si)__A, (__v8si)__B);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_dpbusd_avx_epi32(__m128i __S, __m128i __A, __m128i __B)
{
return (__m128i)__builtin_ia32_vpdpbusd128((__v4si)__S, (__v4si)__A, (__v4si)__B);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_dpbusds_avx_epi32(__m128i __S, __m128i __A, __m128i __B)
{
return (__m128i)__builtin_ia32_vpdpbusds128((__v4si)__S, (__v4si)__A, (__v4si)__B);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_dpwssd_avx_epi32(__m128i __S, __m128i __A, __m128i __B)
{
return (__m128i)__builtin_ia32_vpdpwssd128((__v4si)__S, (__v4si)__A, (__v4si)__B);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_dpwssds_avx_epi32(__m128i __S, __m128i __A, __m128i __B)
{
return (__m128i)__builtin_ia32_vpdpwssds128((__v4si)__S, (__v4si)__A, (__v4si)__B);
}

#undef __DEFAULT_FN_ATTRS128
#undef __DEFAULT_FN_ATTRS256

#endif
