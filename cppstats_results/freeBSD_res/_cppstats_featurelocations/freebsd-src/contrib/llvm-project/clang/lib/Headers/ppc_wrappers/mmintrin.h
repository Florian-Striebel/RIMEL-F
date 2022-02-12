











#if !defined(NO_WARN_X86_INTRINSICS)

















#error "Please read comment above. Use -DNO_WARN_X86_INTRINSICS to disable this error."

#endif

#if !defined(_MMINTRIN_H_INCLUDED)
#define _MMINTRIN_H_INCLUDED

#if defined(__linux__) && defined(__ppc64__)

#include <altivec.h>


typedef __attribute__((__aligned__(8))) unsigned long long __m64;

typedef __attribute__((__aligned__(8))) union {
__m64 as_m64;
char as_char[8];
signed char as_signed_char[8];
short as_short[4];
int as_int[2];
long long as_long_long;
float as_float[2];
double as_double;
} __m64_union;


extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_empty(void) {

}

extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_empty(void) {

}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi32_si64(int __i) {
return (__m64)(unsigned int)__i;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_from_int(int __i) {
return _mm_cvtsi32_si64(__i);
}


extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_si32(__m64 __i) {
return ((int)__i);
}

extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_to_int(__m64 __i) {
return _mm_cvtsi64_si32(__i);
}




extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_from_int64(long long __i) {
return (__m64)__i;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_m64(long long __i) {
return (__m64)__i;
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64x_si64(long long __i) {
return (__m64)__i;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pi64x(long long __i) {
return (__m64)__i;
}




extern __inline long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_to_int64(__m64 __i) {
return (long long)__i;
}

extern __inline long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtm64_si64(__m64 __i) {
return (long long)__i;
}


extern __inline long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_si64x(__m64 __i) {
return (long long)__i;
}

#if defined(_ARCH_PWR8)



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_pi16(__m64 __m1, __m64 __m2) {
__vector signed short vm1;
__vector signed char vresult;

vm1 = (__vector signed short)(__vector unsigned long long)
#if defined(__LITTLE_ENDIAN__)
{__m1, __m2};
#else
{__m2, __m1};
#endif
vresult = vec_packs(vm1, vm1);
return (__m64)((__vector long long)vresult)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_packsswb(__m64 __m1, __m64 __m2) {
return _mm_packs_pi16(__m1, __m2);
}




extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_pi32(__m64 __m1, __m64 __m2) {
__vector signed int vm1;
__vector signed short vresult;

vm1 = (__vector signed int)(__vector unsigned long long)
#if defined(__LITTLE_ENDIAN__)
{__m1, __m2};
#else
{__m2, __m1};
#endif
vresult = vec_packs(vm1, vm1);
return (__m64)((__vector long long)vresult)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_packssdw(__m64 __m1, __m64 __m2) {
return _mm_packs_pi32(__m1, __m2);
}




extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_pu16(__m64 __m1, __m64 __m2) {
__vector unsigned char r;
__vector signed short vm1 = (__vector signed short)(__vector long long)
#if defined(__LITTLE_ENDIAN__)
{__m1, __m2};
#else
{__m2, __m1};
#endif
const __vector signed short __zero = {0};
__vector __bool short __select = vec_cmplt(vm1, __zero);
r = vec_packs((__vector unsigned short)vm1, (__vector unsigned short)vm1);
__vector __bool char packsel = vec_pack(__select, __select);
r = vec_sel(r, (const __vector unsigned char)__zero, packsel);
return (__m64)((__vector long long)r)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_packuswb(__m64 __m1, __m64 __m2) {
return _mm_packs_pu16(__m1, __m2);
}
#endif



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_pi8(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector unsigned char a, b, c;

a = (__vector unsigned char)vec_splats(__m1);
b = (__vector unsigned char)vec_splats(__m2);
c = vec_mergel(a, b);
return (__m64)((__vector long long)c)[1];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = m1.as_char[4];
res.as_char[1] = m2.as_char[4];
res.as_char[2] = m1.as_char[5];
res.as_char[3] = m2.as_char[5];
res.as_char[4] = m1.as_char[6];
res.as_char[5] = m2.as_char[6];
res.as_char[6] = m1.as_char[7];
res.as_char[7] = m2.as_char[7];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpckhbw(__m64 __m1, __m64 __m2) {
return _mm_unpackhi_pi8(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_pi16(__m64 __m1, __m64 __m2) {
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = m1.as_short[2];
res.as_short[1] = m2.as_short[2];
res.as_short[2] = m1.as_short[3];
res.as_short[3] = m2.as_short[3];

return (__m64)res.as_m64;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpckhwd(__m64 __m1, __m64 __m2) {
return _mm_unpackhi_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_pi32(__m64 __m1, __m64 __m2) {
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = m1.as_int[1];
res.as_int[1] = m2.as_int[1];

return (__m64)res.as_m64;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpckhdq(__m64 __m1, __m64 __m2) {
return _mm_unpackhi_pi32(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_pi8(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector unsigned char a, b, c;

a = (__vector unsigned char)vec_splats(__m1);
b = (__vector unsigned char)vec_splats(__m2);
c = vec_mergel(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = m1.as_char[0];
res.as_char[1] = m2.as_char[0];
res.as_char[2] = m1.as_char[1];
res.as_char[3] = m2.as_char[1];
res.as_char[4] = m1.as_char[2];
res.as_char[5] = m2.as_char[2];
res.as_char[6] = m1.as_char[3];
res.as_char[7] = m2.as_char[3];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpcklbw(__m64 __m1, __m64 __m2) {
return _mm_unpacklo_pi8(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_pi16(__m64 __m1, __m64 __m2) {
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = m1.as_short[0];
res.as_short[1] = m2.as_short[0];
res.as_short[2] = m1.as_short[1];
res.as_short[3] = m2.as_short[1];

return (__m64)res.as_m64;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpcklwd(__m64 __m1, __m64 __m2) {
return _mm_unpacklo_pi16(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_pi32(__m64 __m1, __m64 __m2) {
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = m1.as_int[0];
res.as_int[1] = m2.as_int[0];

return (__m64)res.as_m64;
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_punpckldq(__m64 __m1, __m64 __m2) {
return _mm_unpacklo_pi32(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_pi8(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed char a, b, c;

a = (__vector signed char)vec_splats(__m1);
b = (__vector signed char)vec_splats(__m2);
c = vec_add(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = m1.as_char[0] + m2.as_char[0];
res.as_char[1] = m1.as_char[1] + m2.as_char[1];
res.as_char[2] = m1.as_char[2] + m2.as_char[2];
res.as_char[3] = m1.as_char[3] + m2.as_char[3];
res.as_char[4] = m1.as_char[4] + m2.as_char[4];
res.as_char[5] = m1.as_char[5] + m2.as_char[5];
res.as_char[6] = m1.as_char[6] + m2.as_char[6];
res.as_char[7] = m1.as_char[7] + m2.as_char[7];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddb(__m64 __m1, __m64 __m2) {
return _mm_add_pi8(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_pi16(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = vec_add(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = m1.as_short[0] + m2.as_short[0];
res.as_short[1] = m1.as_short[1] + m2.as_short[1];
res.as_short[2] = m1.as_short[2] + m2.as_short[2];
res.as_short[3] = m1.as_short[3] + m2.as_short[3];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddw(__m64 __m1, __m64 __m2) {
return _mm_add_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_pi32(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR9
__vector signed int a, b, c;

a = (__vector signed int)vec_splats(__m1);
b = (__vector signed int)vec_splats(__m2);
c = vec_add(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = m1.as_int[0] + m2.as_int[0];
res.as_int[1] = m1.as_int[1] + m2.as_int[1];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddd(__m64 __m1, __m64 __m2) {
return _mm_add_pi32(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_pi8(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed char a, b, c;

a = (__vector signed char)vec_splats(__m1);
b = (__vector signed char)vec_splats(__m2);
c = vec_sub(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = m1.as_char[0] - m2.as_char[0];
res.as_char[1] = m1.as_char[1] - m2.as_char[1];
res.as_char[2] = m1.as_char[2] - m2.as_char[2];
res.as_char[3] = m1.as_char[3] - m2.as_char[3];
res.as_char[4] = m1.as_char[4] - m2.as_char[4];
res.as_char[5] = m1.as_char[5] - m2.as_char[5];
res.as_char[6] = m1.as_char[6] - m2.as_char[6];
res.as_char[7] = m1.as_char[7] - m2.as_char[7];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubb(__m64 __m1, __m64 __m2) {
return _mm_sub_pi8(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_pi16(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = vec_sub(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = m1.as_short[0] - m2.as_short[0];
res.as_short[1] = m1.as_short[1] - m2.as_short[1];
res.as_short[2] = m1.as_short[2] - m2.as_short[2];
res.as_short[3] = m1.as_short[3] - m2.as_short[3];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubw(__m64 __m1, __m64 __m2) {
return _mm_sub_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_pi32(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR9
__vector signed int a, b, c;

a = (__vector signed int)vec_splats(__m1);
b = (__vector signed int)vec_splats(__m2);
c = vec_sub(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = m1.as_int[0] - m2.as_int[0];
res.as_int[1] = m1.as_int[1] - m2.as_int[1];

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubd(__m64 __m1, __m64 __m2) {
return _mm_sub_pi32(__m1, __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_si64(__m64 __m1, __m64 __m2) {
return (__m1 + __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_si64(__m64 __m1, __m64 __m2) {
return (__m1 - __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_si64(__m64 __m, __m64 __count) {
return (__m << __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psllq(__m64 __m, __m64 __count) {
return _mm_sll_si64(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_si64(__m64 __m, const int __count) {
return (__m << __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psllqi(__m64 __m, const int __count) {
return _mm_slli_si64(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_si64(__m64 __m, __m64 __count) {
return (__m >> __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrlq(__m64 __m, __m64 __count) {
return _mm_srl_si64(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_si64(__m64 __m, const int __count) {
return (__m >> __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrlqi(__m64 __m, const int __count) {
return _mm_srli_si64(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_and_si64(__m64 __m1, __m64 __m2) {
return (__m1 & __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pand(__m64 __m1, __m64 __m2) {
return _mm_and_si64(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_andnot_si64(__m64 __m1, __m64 __m2) {
return (~__m1 & __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pandn(__m64 __m1, __m64 __m2) {
return _mm_andnot_si64(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_or_si64(__m64 __m1, __m64 __m2) {
return (__m1 | __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_por(__m64 __m1, __m64 __m2) {
return _mm_or_si64(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_xor_si64(__m64 __m1, __m64 __m2) {
return (__m1 ^ __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pxor(__m64 __m1, __m64 __m2) {
return _mm_xor_si64(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_si64(void) {
return (__m64)0;
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_pi8(__m64 __m1, __m64 __m2) {
#if defined(_ARCH_PWR6) && defined(__powerpc64__)
__m64 res;
__asm__("cmpb %0,%1,%2;\n" : "=r"(res) : "r"(__m1), "r"(__m2) :);
return (res);
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = (m1.as_char[0] == m2.as_char[0]) ? -1 : 0;
res.as_char[1] = (m1.as_char[1] == m2.as_char[1]) ? -1 : 0;
res.as_char[2] = (m1.as_char[2] == m2.as_char[2]) ? -1 : 0;
res.as_char[3] = (m1.as_char[3] == m2.as_char[3]) ? -1 : 0;
res.as_char[4] = (m1.as_char[4] == m2.as_char[4]) ? -1 : 0;
res.as_char[5] = (m1.as_char[5] == m2.as_char[5]) ? -1 : 0;
res.as_char[6] = (m1.as_char[6] == m2.as_char[6]) ? -1 : 0;
res.as_char[7] = (m1.as_char[7] == m2.as_char[7]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpeqb(__m64 __m1, __m64 __m2) {
return _mm_cmpeq_pi8(__m1, __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_pi8(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed char a, b, c;

a = (__vector signed char)vec_splats(__m1);
b = (__vector signed char)vec_splats(__m2);
c = (__vector signed char)vec_cmpgt(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_char[0] = (m1.as_char[0] > m2.as_char[0]) ? -1 : 0;
res.as_char[1] = (m1.as_char[1] > m2.as_char[1]) ? -1 : 0;
res.as_char[2] = (m1.as_char[2] > m2.as_char[2]) ? -1 : 0;
res.as_char[3] = (m1.as_char[3] > m2.as_char[3]) ? -1 : 0;
res.as_char[4] = (m1.as_char[4] > m2.as_char[4]) ? -1 : 0;
res.as_char[5] = (m1.as_char[5] > m2.as_char[5]) ? -1 : 0;
res.as_char[6] = (m1.as_char[6] > m2.as_char[6]) ? -1 : 0;
res.as_char[7] = (m1.as_char[7] > m2.as_char[7]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpgtb(__m64 __m1, __m64 __m2) {
return _mm_cmpgt_pi8(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_pi16(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = (__vector signed short)vec_cmpeq(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = (m1.as_short[0] == m2.as_short[0]) ? -1 : 0;
res.as_short[1] = (m1.as_short[1] == m2.as_short[1]) ? -1 : 0;
res.as_short[2] = (m1.as_short[2] == m2.as_short[2]) ? -1 : 0;
res.as_short[3] = (m1.as_short[3] == m2.as_short[3]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpeqw(__m64 __m1, __m64 __m2) {
return _mm_cmpeq_pi16(__m1, __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_pi16(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR8
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = (__vector signed short)vec_cmpgt(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_short[0] = (m1.as_short[0] > m2.as_short[0]) ? -1 : 0;
res.as_short[1] = (m1.as_short[1] > m2.as_short[1]) ? -1 : 0;
res.as_short[2] = (m1.as_short[2] > m2.as_short[2]) ? -1 : 0;
res.as_short[3] = (m1.as_short[3] > m2.as_short[3]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpgtw(__m64 __m1, __m64 __m2) {
return _mm_cmpgt_pi16(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_pi32(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR9
__vector signed int a, b, c;

a = (__vector signed int)vec_splats(__m1);
b = (__vector signed int)vec_splats(__m2);
c = (__vector signed int)vec_cmpeq(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = (m1.as_int[0] == m2.as_int[0]) ? -1 : 0;
res.as_int[1] = (m1.as_int[1] == m2.as_int[1]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpeqd(__m64 __m1, __m64 __m2) {
return _mm_cmpeq_pi32(__m1, __m2);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_pi32(__m64 __m1, __m64 __m2) {
#if _ARCH_PWR9
__vector signed int a, b, c;

a = (__vector signed int)vec_splats(__m1);
b = (__vector signed int)vec_splats(__m2);
c = (__vector signed int)vec_cmpgt(a, b);
return (__m64)((__vector long long)c)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __m1;
m2.as_m64 = __m2;

res.as_int[0] = (m1.as_int[0] > m2.as_int[0]) ? -1 : 0;
res.as_int[1] = (m1.as_int[1] > m2.as_int[1]) ? -1 : 0;

return (__m64)res.as_m64;
#endif
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pcmpgtd(__m64 __m1, __m64 __m2) {
return _mm_cmpgt_pi32(__m1, __m2);
}

#if _ARCH_PWR8


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_pi8(__m64 __m1, __m64 __m2) {
__vector signed char a, b, c;

a = (__vector signed char)vec_splats(__m1);
b = (__vector signed char)vec_splats(__m2);
c = vec_adds(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddsb(__m64 __m1, __m64 __m2) {
return _mm_adds_pi8(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_pi16(__m64 __m1, __m64 __m2) {
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = vec_adds(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddsw(__m64 __m1, __m64 __m2) {
return _mm_adds_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_pu8(__m64 __m1, __m64 __m2) {
__vector unsigned char a, b, c;

a = (__vector unsigned char)vec_splats(__m1);
b = (__vector unsigned char)vec_splats(__m2);
c = vec_adds(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddusb(__m64 __m1, __m64 __m2) {
return _mm_adds_pu8(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_pu16(__m64 __m1, __m64 __m2) {
__vector unsigned short a, b, c;

a = (__vector unsigned short)vec_splats(__m1);
b = (__vector unsigned short)vec_splats(__m2);
c = vec_adds(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_paddusw(__m64 __m1, __m64 __m2) {
return _mm_adds_pu16(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_pi8(__m64 __m1, __m64 __m2) {
__vector signed char a, b, c;

a = (__vector signed char)vec_splats(__m1);
b = (__vector signed char)vec_splats(__m2);
c = vec_subs(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubsb(__m64 __m1, __m64 __m2) {
return _mm_subs_pi8(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_pi16(__m64 __m1, __m64 __m2) {
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = vec_subs(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubsw(__m64 __m1, __m64 __m2) {
return _mm_subs_pi16(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_pu8(__m64 __m1, __m64 __m2) {
__vector unsigned char a, b, c;

a = (__vector unsigned char)vec_splats(__m1);
b = (__vector unsigned char)vec_splats(__m2);
c = vec_subs(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubusb(__m64 __m1, __m64 __m2) {
return _mm_subs_pu8(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_pu16(__m64 __m1, __m64 __m2) {
__vector unsigned short a, b, c;

a = (__vector unsigned short)vec_splats(__m1);
b = (__vector unsigned short)vec_splats(__m2);
c = vec_subs(a, b);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psubusw(__m64 __m1, __m64 __m2) {
return _mm_subs_pu16(__m1, __m2);
}




extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd_pi16(__m64 __m1, __m64 __m2) {
__vector signed short a, b;
__vector signed int c;
__vector signed int zero = {0, 0, 0, 0};

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = vec_vmsumshm(a, b, zero);
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmaddwd(__m64 __m1, __m64 __m2) {
return _mm_madd_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_pi16(__m64 __m1, __m64 __m2) {
__vector signed short a, b;
__vector signed short c;
__vector signed int w0, w1;
__vector unsigned char xform1 = {
#if defined(__LITTLE_ENDIAN__)
0x02, 0x03, 0x12, 0x13, 0x06, 0x07, 0x16, 0x17, 0x0A,
0x0B, 0x1A, 0x1B, 0x0E, 0x0F, 0x1E, 0x1F
#else
0x00, 0x01, 0x10, 0x11, 0x04, 0x05, 0x14, 0x15, 0x00,
0x01, 0x10, 0x11, 0x04, 0x05, 0x14, 0x15
#endif
};

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);

w0 = vec_vmulesh(a, b);
w1 = vec_vmulosh(a, b);
c = (__vector signed short)vec_perm(w0, w1, xform1);

return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmulhw(__m64 __m1, __m64 __m2) {
return _mm_mulhi_pi16(__m1, __m2);
}



extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mullo_pi16(__m64 __m1, __m64 __m2) {
__vector signed short a, b, c;

a = (__vector signed short)vec_splats(__m1);
b = (__vector signed short)vec_splats(__m2);
c = a * b;
return (__m64)((__vector long long)c)[0];
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmullw(__m64 __m1, __m64 __m2) {
return _mm_mullo_pi16(__m1, __m2);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_pi16(__m64 __m, __m64 __count) {
__vector signed short m, r;
__vector unsigned short c;

if (__count <= 15) {
m = (__vector signed short)vec_splats(__m);
c = (__vector unsigned short)vec_splats((unsigned short)__count);
r = vec_sl(m, (__vector unsigned short)c);
return (__m64)((__vector long long)r)[0];
} else
return (0);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psllw(__m64 __m, __m64 __count) {
return _mm_sll_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_pi16(__m64 __m, int __count) {

return _mm_sll_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psllwi(__m64 __m, int __count) {
return _mm_slli_pi16(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_pi32(__m64 __m, __m64 __count) {
__m64_union m, res;

m.as_m64 = __m;

res.as_int[0] = m.as_int[0] << __count;
res.as_int[1] = m.as_int[1] << __count;
return (res.as_m64);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pslld(__m64 __m, __m64 __count) {
return _mm_sll_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_pi32(__m64 __m, int __count) {

return _mm_sll_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pslldi(__m64 __m, int __count) {
return _mm_slli_pi32(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sra_pi16(__m64 __m, __m64 __count) {
__vector signed short m, r;
__vector unsigned short c;

if (__count <= 15) {
m = (__vector signed short)vec_splats(__m);
c = (__vector unsigned short)vec_splats((unsigned short)__count);
r = vec_sra(m, (__vector unsigned short)c);
return (__m64)((__vector long long)r)[0];
} else
return (0);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psraw(__m64 __m, __m64 __count) {
return _mm_sra_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_pi16(__m64 __m, int __count) {

return _mm_sra_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrawi(__m64 __m, int __count) {
return _mm_srai_pi16(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sra_pi32(__m64 __m, __m64 __count) {
__m64_union m, res;

m.as_m64 = __m;

res.as_int[0] = m.as_int[0] >> __count;
res.as_int[1] = m.as_int[1] >> __count;
return (res.as_m64);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrad(__m64 __m, __m64 __count) {
return _mm_sra_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_pi32(__m64 __m, int __count) {

return _mm_sra_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psradi(__m64 __m, int __count) {
return _mm_srai_pi32(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_pi16(__m64 __m, __m64 __count) {
__vector unsigned short m, r;
__vector unsigned short c;

if (__count <= 15) {
m = (__vector unsigned short)vec_splats(__m);
c = (__vector unsigned short)vec_splats((unsigned short)__count);
r = vec_sr(m, (__vector unsigned short)c);
return (__m64)((__vector long long)r)[0];
} else
return (0);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrlw(__m64 __m, __m64 __count) {
return _mm_srl_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_pi16(__m64 __m, int __count) {

return _mm_srl_pi16(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrlwi(__m64 __m, int __count) {
return _mm_srli_pi16(__m, __count);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_pi32(__m64 __m, __m64 __count) {
__m64_union m, res;

m.as_m64 = __m;

res.as_int[0] = (unsigned int)m.as_int[0] >> __count;
res.as_int[1] = (unsigned int)m.as_int[1] >> __count;
return (res.as_m64);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrld(__m64 __m, __m64 __count) {
return _mm_srl_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_pi32(__m64 __m, int __count) {

return _mm_srl_pi32(__m, __count);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psrldi(__m64 __m, int __count) {
return _mm_srli_pi32(__m, __count);
}
#endif


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pi32(int __i1, int __i0) {
__m64_union res;

res.as_int[0] = __i0;
res.as_int[1] = __i1;
return (res.as_m64);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pi16(short __w3, short __w2, short __w1, short __w0) {
__m64_union res;

res.as_short[0] = __w0;
res.as_short[1] = __w1;
res.as_short[2] = __w2;
res.as_short[3] = __w3;
return (res.as_m64);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pi8(char __b7, char __b6, char __b5, char __b4, char __b3,
char __b2, char __b1, char __b0) {
__m64_union res;

res.as_char[0] = __b0;
res.as_char[1] = __b1;
res.as_char[2] = __b2;
res.as_char[3] = __b3;
res.as_char[4] = __b4;
res.as_char[5] = __b5;
res.as_char[6] = __b6;
res.as_char[7] = __b7;
return (res.as_m64);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_pi32(int __i0, int __i1) {
__m64_union res;

res.as_int[0] = __i0;
res.as_int[1] = __i1;
return (res.as_m64);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_pi16(short __w0, short __w1, short __w2, short __w3) {
return _mm_set_pi16(__w3, __w2, __w1, __w0);
}

extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_pi8(char __b0, char __b1, char __b2, char __b3, char __b4,
char __b5, char __b6, char __b7) {
return _mm_set_pi8(__b7, __b6, __b5, __b4, __b3, __b2, __b1, __b0);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_pi32(int __i) {
__m64_union res;

res.as_int[0] = __i;
res.as_int[1] = __i;
return (res.as_m64);
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_pi16(short __w) {
#if _ARCH_PWR9
__vector signed short w;

w = (__vector signed short)vec_splats(__w);
return (__m64)((__vector long long)w)[0];
#else
__m64_union res;

res.as_short[0] = __w;
res.as_short[1] = __w;
res.as_short[2] = __w;
res.as_short[3] = __w;
return (res.as_m64);
#endif
}


extern __inline __m64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_pi8(signed char __b) {
#if _ARCH_PWR8
__vector signed char b;

b = (__vector signed char)vec_splats(__b);
return (__m64)((__vector long long)b)[0];
#else
__m64_union res;

res.as_char[0] = __b;
res.as_char[1] = __b;
res.as_char[2] = __b;
res.as_char[3] = __b;
res.as_char[4] = __b;
res.as_char[5] = __b;
res.as_char[6] = __b;
res.as_char[7] = __b;
return (res.as_m64);
#endif
}

#else
#include_next <mmintrin.h>
#endif

#endif
