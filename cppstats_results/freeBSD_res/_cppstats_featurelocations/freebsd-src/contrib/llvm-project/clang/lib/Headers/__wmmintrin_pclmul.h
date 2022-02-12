








#if !defined(__WMMINTRIN_H)
#error "Never use <__wmmintrin_pclmul.h> directly; include <wmmintrin.h> instead."
#endif

#if !defined(__WMMINTRIN_PCLMUL_H)
#define __WMMINTRIN_PCLMUL_H




























#define _mm_clmulepi64_si128(X, Y, I) ((__m128i)__builtin_ia32_pclmulqdq128((__v2di)(__m128i)(X), (__v2di)(__m128i)(Y), (char)(I)))



#endif
