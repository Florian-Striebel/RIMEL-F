








#if !defined __IMMINTRIN_H
#error "Never use <f16cintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__F16CINTRIN_H)
#define __F16CINTRIN_H


#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("f16c"), __min_vector_width__(128)))

#define __DEFAULT_FN_ATTRS256 __attribute__((__always_inline__, __nodebug__, __target__("f16c"), __min_vector_width__(256)))

















static __inline float __DEFAULT_FN_ATTRS128
_cvtsh_ss(unsigned short __a)
{
__v8hi __v = {(short)__a, 0, 0, 0, 0, 0, 0, 0};
__v4sf __r = __builtin_ia32_vcvtph2ps(__v);
return __r[0];
}























#define _cvtss_sh(a, imm) (unsigned short)(((__v8hi)__builtin_ia32_vcvtps2ph((__v4sf){a, 0, 0, 0}, (imm)))[0])


























#define _mm_cvtps_ph(a, imm) (__m128i)__builtin_ia32_vcvtps2ph((__v4sf)(__m128)(a), (imm))













static __inline __m128 __DEFAULT_FN_ATTRS128
_mm_cvtph_ps(__m128i __a)
{
return (__m128)__builtin_ia32_vcvtph2ps((__v8hi)__a);
}
























#define _mm256_cvtps_ph(a, imm) (__m128i)__builtin_ia32_vcvtps2ph256((__v8sf)(__m256)(a), (imm))














static __inline __m256 __DEFAULT_FN_ATTRS256
_mm256_cvtph_ps(__m128i __a)
{
return (__m256)__builtin_ia32_vcvtph2ps256((__v8hi)__a);
}

#undef __DEFAULT_FN_ATTRS128
#undef __DEFAULT_FN_ATTRS256

#endif
