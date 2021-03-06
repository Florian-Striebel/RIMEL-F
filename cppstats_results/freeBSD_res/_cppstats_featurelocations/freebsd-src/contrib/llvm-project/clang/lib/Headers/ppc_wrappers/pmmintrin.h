











#if !defined(NO_WARN_X86_INTRINSICS)





















#error "Please read comment above. Use -DNO_WARN_X86_INTRINSICS to disable this warning."
#endif

#if !defined(PMMINTRIN_H_)
#define PMMINTRIN_H_

#if defined(__linux__) && defined(__ppc64__)


#include <emmintrin.h>

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_addsub_ps (__m128 __X, __m128 __Y)
{
const __v4sf even_n0 = {-0.0, 0.0, -0.0, 0.0};
__v4sf even_neg_Y = vec_xor(__Y, even_n0);
return (__m128) vec_add (__X, even_neg_Y);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_addsub_pd (__m128d __X, __m128d __Y)
{
const __v2df even_n0 = {-0.0, 0.0};
__v2df even_neg_Y = vec_xor(__Y, even_n0);
return (__m128d) vec_add (__X, even_neg_Y);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_hadd_ps (__m128 __X, __m128 __Y)
{
__vector unsigned char xform2 = {
0x00, 0x01, 0x02, 0x03,
0x08, 0x09, 0x0A, 0x0B,
0x10, 0x11, 0x12, 0x13,
0x18, 0x19, 0x1A, 0x1B
};
__vector unsigned char xform1 = {
0x04, 0x05, 0x06, 0x07,
0x0C, 0x0D, 0x0E, 0x0F,
0x14, 0x15, 0x16, 0x17,
0x1C, 0x1D, 0x1E, 0x1F
};
return (__m128) vec_add (vec_perm ((__v4sf) __X, (__v4sf) __Y, xform2),
vec_perm ((__v4sf) __X, (__v4sf) __Y, xform1));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_hsub_ps (__m128 __X, __m128 __Y)
{
__vector unsigned char xform2 = {
0x00, 0x01, 0x02, 0x03,
0x08, 0x09, 0x0A, 0x0B,
0x10, 0x11, 0x12, 0x13,
0x18, 0x19, 0x1A, 0x1B
};
__vector unsigned char xform1 = {
0x04, 0x05, 0x06, 0x07,
0x0C, 0x0D, 0x0E, 0x0F,
0x14, 0x15, 0x16, 0x17,
0x1C, 0x1D, 0x1E, 0x1F
};
return (__m128) vec_sub (vec_perm ((__v4sf) __X, (__v4sf) __Y, xform2),
vec_perm ((__v4sf) __X, (__v4sf) __Y, xform1));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_hadd_pd (__m128d __X, __m128d __Y)
{
return (__m128d) vec_add (vec_mergeh ((__v2df) __X, (__v2df)__Y),
vec_mergel ((__v2df) __X, (__v2df)__Y));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_hsub_pd (__m128d __X, __m128d __Y)
{
return (__m128d) vec_sub (vec_mergeh ((__v2df) __X, (__v2df)__Y),
vec_mergel ((__v2df) __X, (__v2df)__Y));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movehdup_ps (__m128 __X)
{
return (__m128)vec_mergeo ((__v4su)__X, (__v4su)__X);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_moveldup_ps (__m128 __X)
{
return (__m128)vec_mergee ((__v4su)__X, (__v4su)__X);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loaddup_pd (double const *__P)
{
return (__m128d) vec_splats (*__P);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movedup_pd (__m128d __X)
{
return _mm_shuffle_pd (__X, __X, _MM_SHUFFLE2 (0,0));
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_lddqu_si128 (__m128i const *__P)
{
return (__m128i) (vec_vsx_ld(0, (signed int const *)__P));
}



#else
#include_next <pmmintrin.h>
#endif

#endif
