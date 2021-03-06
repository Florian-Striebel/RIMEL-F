








#if !defined(__PMMINTRIN_H)
#define __PMMINTRIN_H

#include <emmintrin.h>


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("sse3"), __min_vector_width__(128)))
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_lddqu_si128(__m128i const *__p)
{
return (__m128i)__builtin_ia32_lddqu((char const *)__p);
}














static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_addsub_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_addsubps((__v4sf)__a, (__v4sf)__b);
}


















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_hadd_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_haddps((__v4sf)__a, (__v4sf)__b);
}


















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_hsub_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_hsubps((__v4sf)__a, (__v4sf)__b);
}

















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_movehdup_ps(__m128 __a)
{
return __builtin_shufflevector((__v4sf)__a, (__v4sf)__a, 1, 1, 3, 3);
}
















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_moveldup_ps(__m128 __a)
{
return __builtin_shufflevector((__v4sf)__a, (__v4sf)__a, 0, 0, 2, 2);
}














static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_addsub_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_addsubpd((__v2df)__a, (__v2df)__b);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_hadd_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_haddpd((__v2df)__a, (__v2df)__b);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_hsub_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_hsubpd((__v2df)__a, (__v2df)__b);
}
















#define _mm_loaddup_pd(dp) _mm_load1_pd(dp)














static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_movedup_pd(__m128d __a)
{
return __builtin_shufflevector((__v2df)__a, (__v2df)__a, 0, 0);
}
















static __inline__ void __DEFAULT_FN_ATTRS
_mm_monitor(void const *__p, unsigned __extensions, unsigned __hints)
{
__builtin_ia32_monitor(__p, __extensions, __hints);
}














static __inline__ void __DEFAULT_FN_ATTRS
_mm_mwait(unsigned __extensions, unsigned __hints)
{
__builtin_ia32_mwait(__extensions, __hints);
}

#undef __DEFAULT_FN_ATTRS

#endif
