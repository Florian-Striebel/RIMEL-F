








#if !defined(__TMMINTRIN_H)
#define __TMMINTRIN_H

#include <pmmintrin.h>


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("ssse3"), __min_vector_width__(64)))
#define __DEFAULT_FN_ATTRS_MMX __attribute__((__always_inline__, __nodebug__, __target__("mmx,ssse3"), __min_vector_width__(64)))













static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_abs_pi8(__m64 __a)
{
return (__m64)__builtin_ia32_pabsb((__v8qi)__a);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_abs_epi8(__m128i __a)
{
return (__m128i)__builtin_ia32_pabsb128((__v16qi)__a);
}













static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_abs_pi16(__m64 __a)
{
return (__m64)__builtin_ia32_pabsw((__v4hi)__a);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_abs_epi16(__m128i __a)
{
return (__m128i)__builtin_ia32_pabsw128((__v8hi)__a);
}













static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_abs_pi32(__m64 __a)
{
return (__m64)__builtin_ia32_pabsd((__v2si)__a);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_abs_epi32(__m128i __a)
{
return (__m128i)__builtin_ia32_pabsd128((__v4si)__a);
}





















#define _mm_alignr_epi8(a, b, n) (__m128i)__builtin_ia32_palignr128((__v16qi)(__m128i)(a), (__v16qi)(__m128i)(b), (n))






















#define _mm_alignr_pi8(a, b, n) (__m64)__builtin_ia32_palignr((__v8qi)(__m64)(a), (__v8qi)(__m64)(b), (n))



















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hadd_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phaddw128((__v8hi)__a, (__v8hi)__b);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hadd_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phaddd128((__v4si)__a, (__v4si)__b);
}


















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hadd_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phaddw((__v4hi)__a, (__v4hi)__b);
}


















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hadd_pi32(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phaddd((__v2si)__a, (__v2si)__b);
}




















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hadds_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phaddsw128((__v8hi)__a, (__v8hi)__b);
}




















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hadds_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phaddsw((__v4hi)__a, (__v4hi)__b);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hsub_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phsubw128((__v8hi)__a, (__v8hi)__b);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hsub_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phsubd128((__v4si)__a, (__v4si)__b);
}


















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hsub_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phsubw((__v4hi)__a, (__v4hi)__b);
}


















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hsub_pi32(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phsubd((__v2si)__a, (__v2si)__b);
}




















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_hsubs_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_phsubsw128((__v8hi)__a, (__v8hi)__b);
}




















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_hsubs_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_phsubsw((__v4hi)__a, (__v4hi)__b);
}





























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_maddubs_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmaddubsw128((__v16qi)__a, (__v16qi)__b);
}

























static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_maddubs_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_pmaddubsw((__v8qi)__a, (__v8qi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mulhrs_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmulhrsw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_mulhrs_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_pmulhrsw((__v4hi)__a, (__v4hi)__b);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_shuffle_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pshufb128((__v16qi)__a, (__v16qi)__b);
}




















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_shuffle_pi8(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_pshufb((__v8qi)__a, (__v8qi)__b);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sign_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psignb128((__v16qi)__a, (__v16qi)__b);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sign_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psignw128((__v8hi)__a, (__v8hi)__b);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sign_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psignd128((__v4si)__a, (__v4si)__b);
}





















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_sign_pi8(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_psignb((__v8qi)__a, (__v8qi)__b);
}





















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_sign_pi16(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_psignw((__v4hi)__a, (__v4hi)__b);
}





















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_sign_pi32(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_psignd((__v2si)__a, (__v2si)__b);
}

#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS_MMX

#endif
