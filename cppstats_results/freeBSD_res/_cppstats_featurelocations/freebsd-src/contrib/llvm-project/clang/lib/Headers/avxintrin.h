








#if !defined(__IMMINTRIN_H)
#error "Never use <avxintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__AVXINTRIN_H)
#define __AVXINTRIN_H

typedef double __v4df __attribute__ ((__vector_size__ (32)));
typedef float __v8sf __attribute__ ((__vector_size__ (32)));
typedef long long __v4di __attribute__ ((__vector_size__ (32)));
typedef int __v8si __attribute__ ((__vector_size__ (32)));
typedef short __v16hi __attribute__ ((__vector_size__ (32)));
typedef char __v32qi __attribute__ ((__vector_size__ (32)));


typedef unsigned long long __v4du __attribute__ ((__vector_size__ (32)));
typedef unsigned int __v8su __attribute__ ((__vector_size__ (32)));
typedef unsigned short __v16hu __attribute__ ((__vector_size__ (32)));
typedef unsigned char __v32qu __attribute__ ((__vector_size__ (32)));



typedef signed char __v32qs __attribute__((__vector_size__(32)));

typedef float __m256 __attribute__ ((__vector_size__ (32), __aligned__(32)));
typedef double __m256d __attribute__((__vector_size__(32), __aligned__(32)));
typedef long long __m256i __attribute__((__vector_size__(32), __aligned__(32)));

typedef float __m256_u __attribute__ ((__vector_size__ (32), __aligned__(1)));
typedef double __m256d_u __attribute__((__vector_size__(32), __aligned__(1)));
typedef long long __m256i_u __attribute__((__vector_size__(32), __aligned__(1)));


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("avx"), __min_vector_width__(256)))
#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avx"), __min_vector_width__(128)))














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_add_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4df)__a+(__v4df)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_add_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8sf)__a+(__v8sf)__b);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_sub_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4df)__a-(__v4df)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_sub_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8sf)__a-(__v8sf)__b);
}














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_addsub_pd(__m256d __a, __m256d __b)
{
return (__m256d)__builtin_ia32_addsubpd256((__v4df)__a, (__v4df)__b);
}














static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_addsub_ps(__m256 __a, __m256 __b)
{
return (__m256)__builtin_ia32_addsubps256((__v8sf)__a, (__v8sf)__b);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_div_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4df)__a/(__v4df)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_div_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8sf)__a/(__v8sf)__b);
}














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_max_pd(__m256d __a, __m256d __b)
{
return (__m256d)__builtin_ia32_maxpd256((__v4df)__a, (__v4df)__b);
}














static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_max_ps(__m256 __a, __m256 __b)
{
return (__m256)__builtin_ia32_maxps256((__v8sf)__a, (__v8sf)__b);
}














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_min_pd(__m256d __a, __m256d __b)
{
return (__m256d)__builtin_ia32_minpd256((__v4df)__a, (__v4df)__b);
}














static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_min_ps(__m256 __a, __m256 __b)
{
return (__m256)__builtin_ia32_minps256((__v8sf)__a, (__v8sf)__b);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_mul_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4df)__a * (__v4df)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_mul_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8sf)__a * (__v8sf)__b);
}












static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_sqrt_pd(__m256d __a)
{
return (__m256d)__builtin_ia32_sqrtpd256((__v4df)__a);
}












static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_sqrt_ps(__m256 __a)
{
return (__m256)__builtin_ia32_sqrtps256((__v8sf)__a);
}












static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_rsqrt_ps(__m256 __a)
{
return (__m256)__builtin_ia32_rsqrtps256((__v8sf)__a);
}












static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_rcp_ps(__m256 __a)
{
return (__m256)__builtin_ia32_rcpps256((__v8sf)__a);
}






























#define _mm256_round_pd(V, M) (__m256d)__builtin_ia32_roundpd256((__v4df)(__m256d)(V), (M))































#define _mm256_round_ps(V, M) (__m256)__builtin_ia32_roundps256((__v8sf)(__m256)(V), (M))

















#define _mm256_ceil_pd(V) _mm256_round_pd((V), _MM_FROUND_CEIL)

















#define _mm256_floor_pd(V) _mm256_round_pd((V), _MM_FROUND_FLOOR)
















#define _mm256_ceil_ps(V) _mm256_round_ps((V), _MM_FROUND_CEIL)
















#define _mm256_floor_ps(V) _mm256_round_ps((V), _MM_FROUND_FLOOR)














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_and_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4du)__a & (__v4du)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_and_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8su)__a & (__v8su)__b);
}
















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_andnot_pd(__m256d __a, __m256d __b)
{
return (__m256d)(~(__v4du)__a & (__v4du)__b);
}
















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_andnot_ps(__m256 __a, __m256 __b)
{
return (__m256)(~(__v8su)__a & (__v8su)__b);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_or_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4du)__a | (__v4du)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_or_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8su)__a | (__v8su)__b);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_xor_pd(__m256d __a, __m256d __b)
{
return (__m256d)((__v4du)__a ^ (__v4du)__b);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_xor_ps(__m256 __a, __m256 __b)
{
return (__m256)((__v8su)__a ^ (__v8su)__b);
}



















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_hadd_pd(__m256d __a, __m256d __b)
{
return (__m256d)__builtin_ia32_haddpd256((__v4df)__a, (__v4df)__b);
}


















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_hadd_ps(__m256 __a, __m256 __b)
{
return (__m256)__builtin_ia32_haddps256((__v8sf)__a, (__v8sf)__b);
}


















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_hsub_pd(__m256d __a, __m256d __b)
{
return (__m256d)__builtin_ia32_hsubpd256((__v4df)__a, (__v4df)__b);
}


















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_hsub_ps(__m256 __a, __m256 __b)
{
return (__m256)__builtin_ia32_hsubps256((__v8sf)__a, (__v8sf)__b);
}

























static __inline __m128d __DEFAULT_FN_ATTRS128
_mm_permutevar_pd(__m128d __a, __m128i __c)
{
return (__m128d)__builtin_ia32_vpermilvarpd((__v2df)__a, (__v2di)__c);
}


































static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_permutevar_pd(__m256d __a, __m256i __c)
{
return (__m256d)__builtin_ia32_vpermilvarpd256((__v4df)__a, (__v4di)__c);
}

















































static __inline __m128 __DEFAULT_FN_ATTRS128
_mm_permutevar_ps(__m128 __a, __m128i __c)
{
return (__m128)__builtin_ia32_vpermilvarps((__v4sf)__a, (__v4si)__c);
}






















































































static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_permutevar_ps(__m256 __a, __m256i __c)
{
return (__m256)__builtin_ia32_vpermilvarps256((__v8sf)__a, (__v8si)__c);
}




























#define _mm_permute_pd(A, C) (__m128d)__builtin_ia32_vpermilpd((__v2df)(__m128d)(A), (int)(C))







































#define _mm256_permute_pd(A, C) (__m256d)__builtin_ia32_vpermilpd256((__v4df)(__m256d)(A), (int)(C))























































#define _mm_permute_ps(A, C) (__m128)__builtin_ia32_vpermilps((__v4sf)(__m128)(A), (int)(C))



























































































#define _mm256_permute_ps(A, C) (__m256)__builtin_ia32_vpermilps256((__v8sf)(__m256)(A), (int)(C))







































#define _mm256_permute2f128_pd(V1, V2, M) (__m256d)__builtin_ia32_vperm2f128_pd256((__v4df)(__m256d)(V1), (__v4df)(__m256d)(V2), (int)(M))








































#define _mm256_permute2f128_ps(V1, V2, M) (__m256)__builtin_ia32_vperm2f128_ps256((__v8sf)(__m256)(V1), (__v8sf)(__m256)(V2), (int)(M))







































#define _mm256_permute2f128_si256(V1, V2, M) (__m256i)__builtin_ia32_vperm2f128_si256((__v8si)(__m256i)(V1), (__v8si)(__m256i)(V2), (int)(M))




























#define _mm256_blend_pd(V1, V2, M) (__m256d)__builtin_ia32_blendpd256((__v4df)(__m256d)(V1), (__v4df)(__m256d)(V2), (int)(M))



























#define _mm256_blend_ps(V1, V2, M) (__m256)__builtin_ia32_blendps256((__v8sf)(__m256)(V1), (__v8sf)(__m256)(V2), (int)(M))
























static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_blendv_pd(__m256d __a, __m256d __b, __m256d __c)
{
return (__m256d)__builtin_ia32_blendvpd256(
(__v4df)__a, (__v4df)__b, (__v4df)__c);
}






















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_blendv_ps(__m256 __a, __m256 __b, __m256 __c)
{
return (__m256)__builtin_ia32_blendvps256(
(__v8sf)__a, (__v8sf)__b, (__v8sf)__c);
}







































#define _mm256_dp_ps(V1, V2, M) (__m256)__builtin_ia32_dpps256((__v8sf)(__m256)(V1), (__v8sf)(__m256)(V2), (M))





















































#define _mm256_shuffle_ps(a, b, mask) (__m256)__builtin_ia32_shufps256((__v8sf)(__m256)(a), (__v8sf)(__m256)(b), (int)(mask))













































#define _mm256_shuffle_pd(a, b, mask) (__m256d)__builtin_ia32_shufpd256((__v4df)(__m256d)(a), (__v4df)(__m256d)(b), (int)(mask))




#define _CMP_EQ_OQ 0x00
#define _CMP_LT_OS 0x01
#define _CMP_LE_OS 0x02
#define _CMP_UNORD_Q 0x03
#define _CMP_NEQ_UQ 0x04
#define _CMP_NLT_US 0x05
#define _CMP_NLE_US 0x06
#define _CMP_ORD_Q 0x07
#define _CMP_EQ_UQ 0x08
#define _CMP_NGE_US 0x09
#define _CMP_NGT_US 0x0a
#define _CMP_FALSE_OQ 0x0b
#define _CMP_NEQ_OQ 0x0c
#define _CMP_GE_OS 0x0d
#define _CMP_GT_OS 0x0e
#define _CMP_TRUE_UQ 0x0f
#define _CMP_EQ_OS 0x10
#define _CMP_LT_OQ 0x11
#define _CMP_LE_OQ 0x12
#define _CMP_UNORD_S 0x13
#define _CMP_NEQ_US 0x14
#define _CMP_NLT_UQ 0x15
#define _CMP_NLE_UQ 0x16
#define _CMP_ORD_S 0x17
#define _CMP_EQ_US 0x18
#define _CMP_NGE_UQ 0x19
#define _CMP_NGT_UQ 0x1a
#define _CMP_FALSE_OS 0x1b
#define _CMP_NEQ_OS 0x1c
#define _CMP_GE_OQ 0x1d
#define _CMP_GT_OQ 0x1e
#define _CMP_TRUE_US 0x1f

























































#define _mm_cmp_pd(a, b, c) (__m128d)__builtin_ia32_cmppd((__v2df)(__m128d)(a), (__v2df)(__m128d)(b), (c))



























































#define _mm_cmp_ps(a, b, c) (__m128)__builtin_ia32_cmpps((__v4sf)(__m128)(a), (__v4sf)(__m128)(b), (c))



























































#define _mm256_cmp_pd(a, b, c) (__m256d)__builtin_ia32_cmppd256((__v4df)(__m256d)(a), (__v4df)(__m256d)(b), (c))



























































#define _mm256_cmp_ps(a, b, c) (__m256)__builtin_ia32_cmpps256((__v8sf)(__m256)(a), (__v8sf)(__m256)(b), (c))


























































#define _mm_cmp_sd(a, b, c) (__m128d)__builtin_ia32_cmpsd((__v2df)(__m128d)(a), (__v2df)(__m128d)(b), (c))


























































#define _mm_cmp_ss(a, b, c) (__m128)__builtin_ia32_cmpss((__v4sf)(__m128)(a), (__v4sf)(__m128)(b), (c))


















#define _mm256_extract_epi32(X, N) (int)__builtin_ia32_vec_ext_v8si((__v8si)(__m256i)(X), (int)(N))

















#define _mm256_extract_epi16(X, N) (int)(unsigned short)__builtin_ia32_vec_ext_v16hi((__v16hi)(__m256i)(X), (int)(N))


















#define _mm256_extract_epi8(X, N) (int)(unsigned char)__builtin_ia32_vec_ext_v32qi((__v32qi)(__m256i)(X), (int)(N))



#if defined(__x86_64__)















#define _mm256_extract_epi64(X, N) (long long)__builtin_ia32_vec_ext_v4di((__v4di)(__m256i)(X), (int)(N))

#endif



















#define _mm256_insert_epi32(X, I, N) (__m256i)__builtin_ia32_vec_set_v8si((__v8si)(__m256i)(X), (int)(I), (int)(N))






















#define _mm256_insert_epi16(X, I, N) (__m256i)__builtin_ia32_vec_set_v16hi((__v16hi)(__m256i)(X), (int)(I), (int)(N))





















#define _mm256_insert_epi8(X, I, N) (__m256i)__builtin_ia32_vec_set_v32qi((__v32qi)(__m256i)(X), (int)(I), (int)(N))



#if defined(__x86_64__)


















#define _mm256_insert_epi64(X, I, N) (__m256i)__builtin_ia32_vec_set_v4di((__v4di)(__m256i)(X), (long long)(I), (int)(N))


#endif











static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_cvtepi32_pd(__m128i __a)
{
return (__m256d)__builtin_convertvector((__v4si)__a, __v4df);
}










static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_cvtepi32_ps(__m256i __a)
{
return (__m256)__builtin_convertvector((__v8si)__a, __v8sf);
}











static __inline __m128 __DEFAULT_FN_ATTRS
_mm256_cvtpd_ps(__m256d __a)
{
return (__m128)__builtin_ia32_cvtpd2ps256((__v4df) __a);
}










static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_cvtps_epi32(__m256 __a)
{
return (__m256i)__builtin_ia32_cvtps2dq256((__v8sf) __a);
}











static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_cvtps_pd(__m128 __a)
{
return (__m256d)__builtin_convertvector((__v4sf)__a, __v4df);
}












static __inline __m128i __DEFAULT_FN_ATTRS
_mm256_cvttpd_epi32(__m256d __a)
{
return (__m128i)__builtin_ia32_cvttpd2dq256((__v4df) __a);
}












static __inline __m128i __DEFAULT_FN_ATTRS
_mm256_cvtpd_epi32(__m256d __a)
{
return (__m128i)__builtin_ia32_cvtpd2dq256((__v4df) __a);
}











static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_cvttps_epi32(__m256 __a)
{
return (__m256i)__builtin_ia32_cvttps2dq256((__v8sf) __a);
}











static __inline double __DEFAULT_FN_ATTRS
_mm256_cvtsd_f64(__m256d __a)
{
return __a[0];
}











static __inline int __DEFAULT_FN_ATTRS
_mm256_cvtsi256_si32(__m256i __a)
{
__v8si __b = (__v8si)__a;
return __b[0];
}











static __inline float __DEFAULT_FN_ATTRS
_mm256_cvtss_f32(__m256 __a)
{
return __a[0];
}





















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_movehdup_ps(__m256 __a)
{
return __builtin_shufflevector((__v8sf)__a, (__v8sf)__a, 1, 1, 3, 3, 5, 5, 7, 7);
}




















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_moveldup_ps(__m256 __a)
{
return __builtin_shufflevector((__v8sf)__a, (__v8sf)__a, 0, 0, 2, 2, 4, 4, 6, 6);
}

















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_movedup_pd(__m256d __a)
{
return __builtin_shufflevector((__v4df)__a, (__v4df)__a, 0, 0, 2, 2);
}


















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_unpackhi_pd(__m256d __a, __m256d __b)
{
return __builtin_shufflevector((__v4df)__a, (__v4df)__b, 1, 5, 1+2, 5+2);
}

















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_unpacklo_pd(__m256d __a, __m256d __b)
{
return __builtin_shufflevector((__v4df)__a, (__v4df)__b, 0, 4, 0+2, 4+2);
}






















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_unpackhi_ps(__m256 __a, __m256 __b)
{
return __builtin_shufflevector((__v8sf)__a, (__v8sf)__b, 2, 10, 2+1, 10+1, 6, 14, 6+1, 14+1);
}






















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_unpacklo_ps(__m256 __a, __m256 __b)
{
return __builtin_shufflevector((__v8sf)__a, (__v8sf)__b, 0, 8, 0+1, 8+1, 4, 12, 4+1, 12+1);
}

























static __inline int __DEFAULT_FN_ATTRS128
_mm_testz_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_vtestzpd((__v2df)__a, (__v2df)__b);
}
























static __inline int __DEFAULT_FN_ATTRS128
_mm_testc_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_vtestcpd((__v2df)__a, (__v2df)__b);
}

























static __inline int __DEFAULT_FN_ATTRS128
_mm_testnzc_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_vtestnzcpd((__v2df)__a, (__v2df)__b);
}
























static __inline int __DEFAULT_FN_ATTRS128
_mm_testz_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_vtestzps((__v4sf)__a, (__v4sf)__b);
}
























static __inline int __DEFAULT_FN_ATTRS128
_mm_testc_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_vtestcps((__v4sf)__a, (__v4sf)__b);
}

























static __inline int __DEFAULT_FN_ATTRS128
_mm_testnzc_ps(__m128 __a, __m128 __b)
{
return __builtin_ia32_vtestnzcps((__v4sf)__a, (__v4sf)__b);
}
























static __inline int __DEFAULT_FN_ATTRS
_mm256_testz_pd(__m256d __a, __m256d __b)
{
return __builtin_ia32_vtestzpd256((__v4df)__a, (__v4df)__b);
}
























static __inline int __DEFAULT_FN_ATTRS
_mm256_testc_pd(__m256d __a, __m256d __b)
{
return __builtin_ia32_vtestcpd256((__v4df)__a, (__v4df)__b);
}

























static __inline int __DEFAULT_FN_ATTRS
_mm256_testnzc_pd(__m256d __a, __m256d __b)
{
return __builtin_ia32_vtestnzcpd256((__v4df)__a, (__v4df)__b);
}
























static __inline int __DEFAULT_FN_ATTRS
_mm256_testz_ps(__m256 __a, __m256 __b)
{
return __builtin_ia32_vtestzps256((__v8sf)__a, (__v8sf)__b);
}
























static __inline int __DEFAULT_FN_ATTRS
_mm256_testc_ps(__m256 __a, __m256 __b)
{
return __builtin_ia32_vtestcps256((__v8sf)__a, (__v8sf)__b);
}

























static __inline int __DEFAULT_FN_ATTRS
_mm256_testnzc_ps(__m256 __a, __m256 __b)
{
return __builtin_ia32_vtestnzcps256((__v8sf)__a, (__v8sf)__b);
}





















static __inline int __DEFAULT_FN_ATTRS
_mm256_testz_si256(__m256i __a, __m256i __b)
{
return __builtin_ia32_ptestz256((__v4di)__a, (__v4di)__b);
}





















static __inline int __DEFAULT_FN_ATTRS
_mm256_testc_si256(__m256i __a, __m256i __b)
{
return __builtin_ia32_ptestc256((__v4di)__a, (__v4di)__b);
}






















static __inline int __DEFAULT_FN_ATTRS
_mm256_testnzc_si256(__m256i __a, __m256i __b)
{
return __builtin_ia32_ptestnzc256((__v4di)__a, (__v4di)__b);
}














static __inline int __DEFAULT_FN_ATTRS
_mm256_movemask_pd(__m256d __a)
{
return __builtin_ia32_movmskpd256((__v4df)__a);
}













static __inline int __DEFAULT_FN_ATTRS
_mm256_movemask_ps(__m256 __a)
{
return __builtin_ia32_movmskps256((__v8sf)__a);
}







static __inline void __attribute__((__always_inline__, __nodebug__, __target__("avx")))
_mm256_zeroall(void)
{
__builtin_ia32_vzeroall();
}






static __inline void __attribute__((__always_inline__, __nodebug__, __target__("avx")))
_mm256_zeroupper(void)
{
__builtin_ia32_vzeroupper();
}














static __inline __m128 __DEFAULT_FN_ATTRS128
_mm_broadcast_ss(float const *__a)
{
float __f = *__a;
return __extension__ (__m128)(__v4sf){ __f, __f, __f, __f };
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_broadcast_sd(double const *__a)
{
double __d = *__a;
return __extension__ (__m256d)(__v4df){ __d, __d, __d, __d };
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_broadcast_ss(float const *__a)
{
float __f = *__a;
return __extension__ (__m256)(__v8sf){ __f, __f, __f, __f, __f, __f, __f, __f };
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_broadcast_pd(__m128d const *__a)
{
__m128d __b = _mm_loadu_pd((const double *)__a);
return (__m256d)__builtin_shufflevector((__v2df)__b, (__v2df)__b,
0, 1, 0, 1);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_broadcast_ps(__m128 const *__a)
{
__m128 __b = _mm_loadu_ps((const float *)__a);
return (__m256)__builtin_shufflevector((__v4sf)__b, (__v4sf)__b,
0, 1, 2, 3, 0, 1, 2, 3);
}













static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_load_pd(double const *__p)
{
return *(const __m256d *)__p;
}











static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_load_ps(float const *__p)
{
return *(const __m256 *)__p;
}












static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_loadu_pd(double const *__p)
{
struct __loadu_pd {
__m256d_u __v;
} __attribute__((__packed__, __may_alias__));
return ((const struct __loadu_pd*)__p)->__v;
}












static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_loadu_ps(float const *__p)
{
struct __loadu_ps {
__m256_u __v;
} __attribute__((__packed__, __may_alias__));
return ((const struct __loadu_ps*)__p)->__v;
}












static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_load_si256(__m256i const *__p)
{
return *__p;
}











static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_loadu_si256(__m256i_u const *__p)
{
struct __loadu_si256 {
__m256i_u __v;
} __attribute__((__packed__, __may_alias__));
return ((const struct __loadu_si256*)__p)->__v;
}













static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_lddqu_si256(__m256i const *__p)
{
return (__m256i)__builtin_ia32_lddqu256((char const *)__p);
}















static __inline void __DEFAULT_FN_ATTRS
_mm256_store_pd(double *__p, __m256d __a)
{
*(__m256d *)__p = __a;
}













static __inline void __DEFAULT_FN_ATTRS
_mm256_store_ps(float *__p, __m256 __a)
{
*(__m256 *)__p = __a;
}













static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu_pd(double *__p, __m256d __a)
{
struct __storeu_pd {
__m256d_u __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_pd*)__p)->__v = __a;
}












static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu_ps(float *__p, __m256 __a)
{
struct __storeu_ps {
__m256_u __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_ps*)__p)->__v = __a;
}













static __inline void __DEFAULT_FN_ATTRS
_mm256_store_si256(__m256i *__p, __m256i __a)
{
*__p = __a;
}












static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu_si256(__m256i_u *__p, __m256i __a)
{
struct __storeu_si256 {
__m256i_u __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_si256*)__p)->__v = __a;
}




















static __inline __m128d __DEFAULT_FN_ATTRS128
_mm_maskload_pd(double const *__p, __m128i __m)
{
return (__m128d)__builtin_ia32_maskloadpd((const __v2df *)__p, (__v2di)__m);
}



















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_maskload_pd(double const *__p, __m256i __m)
{
return (__m256d)__builtin_ia32_maskloadpd256((const __v4df *)__p,
(__v4di)__m);
}



















static __inline __m128 __DEFAULT_FN_ATTRS128
_mm_maskload_ps(float const *__p, __m128i __m)
{
return (__m128)__builtin_ia32_maskloadps((const __v4sf *)__p, (__v4si)__m);
}



















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_maskload_ps(float const *__p, __m256i __m)
{
return (__m256)__builtin_ia32_maskloadps256((const __v8sf *)__p, (__v8si)__m);
}




















static __inline void __DEFAULT_FN_ATTRS
_mm256_maskstore_ps(float *__p, __m256i __m, __m256 __a)
{
__builtin_ia32_maskstoreps256((__v8sf *)__p, (__v8si)__m, (__v8sf)__a);
}



















static __inline void __DEFAULT_FN_ATTRS128
_mm_maskstore_pd(double *__p, __m128i __m, __m128d __a)
{
__builtin_ia32_maskstorepd((__v2df *)__p, (__v2di)__m, (__v2df)__a);
}



















static __inline void __DEFAULT_FN_ATTRS
_mm256_maskstore_pd(double *__p, __m256i __m, __m256d __a)
{
__builtin_ia32_maskstorepd256((__v4df *)__p, (__v4di)__m, (__v4df)__a);
}



















static __inline void __DEFAULT_FN_ATTRS128
_mm_maskstore_ps(float *__p, __m128i __m, __m128 __a)
{
__builtin_ia32_maskstoreps((__v4sf *)__p, (__v4si)__m, (__v4sf)__a);
}















static __inline void __DEFAULT_FN_ATTRS
_mm256_stream_si256(__m256i *__a, __m256i __b)
{
typedef __v4di __v4di_aligned __attribute__((aligned(32)));
__builtin_nontemporal_store((__v4di_aligned)__b, (__v4di_aligned*)__a);
}














static __inline void __DEFAULT_FN_ATTRS
_mm256_stream_pd(double *__a, __m256d __b)
{
typedef __v4df __v4df_aligned __attribute__((aligned(32)));
__builtin_nontemporal_store((__v4df_aligned)__b, (__v4df_aligned*)__a);
}















static __inline void __DEFAULT_FN_ATTRS
_mm256_stream_ps(float *__p, __m256 __a)
{
typedef __v8sf __v8sf_aligned __attribute__((aligned(32)));
__builtin_nontemporal_store((__v8sf_aligned)__a, (__v8sf_aligned*)__p);
}









static __inline__ __m256d __DEFAULT_FN_ATTRS
_mm256_undefined_pd(void)
{
return (__m256d)__builtin_ia32_undef256();
}








static __inline__ __m256 __DEFAULT_FN_ATTRS
_mm256_undefined_ps(void)
{
return (__m256)__builtin_ia32_undef256();
}








static __inline__ __m256i __DEFAULT_FN_ATTRS
_mm256_undefined_si256(void)
{
return (__m256i)__builtin_ia32_undef256();
}






















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_set_pd(double __a, double __b, double __c, double __d)
{
return __extension__ (__m256d){ __d, __c, __b, __a };
}


































static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_set_ps(float __a, float __b, float __c, float __d,
float __e, float __f, float __g, float __h)
{
return __extension__ (__m256){ __h, __g, __f, __e, __d, __c, __b, __a };
}


























static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set_epi32(int __i0, int __i1, int __i2, int __i3,
int __i4, int __i5, int __i6, int __i7)
{
return __extension__ (__m256i)(__v8si){ __i7, __i6, __i5, __i4, __i3, __i2, __i1, __i0 };
}










































static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set_epi16(short __w15, short __w14, short __w13, short __w12,
short __w11, short __w10, short __w09, short __w08,
short __w07, short __w06, short __w05, short __w04,
short __w03, short __w02, short __w01, short __w00)
{
return __extension__ (__m256i)(__v16hi){ __w00, __w01, __w02, __w03, __w04, __w05, __w06,
__w07, __w08, __w09, __w10, __w11, __w12, __w13, __w14, __w15 };
}










































































static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set_epi8(char __b31, char __b30, char __b29, char __b28,
char __b27, char __b26, char __b25, char __b24,
char __b23, char __b22, char __b21, char __b20,
char __b19, char __b18, char __b17, char __b16,
char __b15, char __b14, char __b13, char __b12,
char __b11, char __b10, char __b09, char __b08,
char __b07, char __b06, char __b05, char __b04,
char __b03, char __b02, char __b01, char __b00)
{
return __extension__ (__m256i)(__v32qi){
__b00, __b01, __b02, __b03, __b04, __b05, __b06, __b07,
__b08, __b09, __b10, __b11, __b12, __b13, __b14, __b15,
__b16, __b17, __b18, __b19, __b20, __b21, __b22, __b23,
__b24, __b25, __b26, __b27, __b28, __b29, __b30, __b31
};
}


















static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set_epi64x(long long __a, long long __b, long long __c, long long __d)
{
return __extension__ (__m256i)(__v4di){ __d, __c, __b, __a };
}
























static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_setr_pd(double __a, double __b, double __c, double __d)
{
return _mm256_set_pd(__d, __c, __b, __a);
}



































static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_setr_ps(float __a, float __b, float __c, float __d,
float __e, float __f, float __g, float __h)
{
return _mm256_set_ps(__h, __g, __f, __e, __d, __c, __b, __a);
}


























static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setr_epi32(int __i0, int __i1, int __i2, int __i3,
int __i4, int __i5, int __i6, int __i7)
{
return _mm256_set_epi32(__i7, __i6, __i5, __i4, __i3, __i2, __i1, __i0);
}










































static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setr_epi16(short __w15, short __w14, short __w13, short __w12,
short __w11, short __w10, short __w09, short __w08,
short __w07, short __w06, short __w05, short __w04,
short __w03, short __w02, short __w01, short __w00)
{
return _mm256_set_epi16(__w00, __w01, __w02, __w03,
__w04, __w05, __w06, __w07,
__w08, __w09, __w10, __w11,
__w12, __w13, __w14, __w15);
}










































































static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setr_epi8(char __b31, char __b30, char __b29, char __b28,
char __b27, char __b26, char __b25, char __b24,
char __b23, char __b22, char __b21, char __b20,
char __b19, char __b18, char __b17, char __b16,
char __b15, char __b14, char __b13, char __b12,
char __b11, char __b10, char __b09, char __b08,
char __b07, char __b06, char __b05, char __b04,
char __b03, char __b02, char __b01, char __b00)
{
return _mm256_set_epi8(__b00, __b01, __b02, __b03, __b04, __b05, __b06, __b07,
__b08, __b09, __b10, __b11, __b12, __b13, __b14, __b15,
__b16, __b17, __b18, __b19, __b20, __b21, __b22, __b23,
__b24, __b25, __b26, __b27, __b28, __b29, __b30, __b31);
}


















static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setr_epi64x(long long __a, long long __b, long long __c, long long __d)
{
return _mm256_set_epi64x(__d, __c, __b, __a);
}














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_set1_pd(double __w)
{
return _mm256_set_pd(__w, __w, __w, __w);
}














static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_set1_ps(float __w)
{
return _mm256_set_ps(__w, __w, __w, __w, __w, __w, __w, __w);
}














static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set1_epi32(int __i)
{
return _mm256_set_epi32(__i, __i, __i, __i, __i, __i, __i, __i);
}













static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set1_epi16(short __w)
{
return _mm256_set_epi16(__w, __w, __w, __w, __w, __w, __w, __w,
__w, __w, __w, __w, __w, __w, __w, __w);
}












static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set1_epi8(char __b)
{
return _mm256_set_epi8(__b, __b, __b, __b, __b, __b, __b, __b,
__b, __b, __b, __b, __b, __b, __b, __b,
__b, __b, __b, __b, __b, __b, __b, __b,
__b, __b, __b, __b, __b, __b, __b, __b);
}













static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set1_epi64x(long long __q)
{
return _mm256_set_epi64x(__q, __q, __q, __q);
}










static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_setzero_pd(void)
{
return __extension__ (__m256d){ 0, 0, 0, 0 };
}









static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_setzero_ps(void)
{
return __extension__ (__m256){ 0, 0, 0, 0, 0, 0, 0, 0 };
}








static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setzero_si256(void)
{
return __extension__ (__m256i)(__v4di){ 0, 0, 0, 0 };
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_castpd_ps(__m256d __a)
{
return (__m256)__a;
}












static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_castpd_si256(__m256d __a)
{
return (__m256i)__a;
}












static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_castps_pd(__m256 __a)
{
return (__m256d)__a;
}












static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_castps_si256(__m256 __a)
{
return (__m256i)__a;
}












static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_castsi256_ps(__m256i __a)
{
return (__m256)__a;
}












static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_castsi256_pd(__m256i __a)
{
return (__m256d)__a;
}












static __inline __m128d __DEFAULT_FN_ATTRS
_mm256_castpd256_pd128(__m256d __a)
{
return __builtin_shufflevector((__v4df)__a, (__v4df)__a, 0, 1);
}












static __inline __m128 __DEFAULT_FN_ATTRS
_mm256_castps256_ps128(__m256 __a)
{
return __builtin_shufflevector((__v8sf)__a, (__v8sf)__a, 0, 1, 2, 3);
}











static __inline __m128i __DEFAULT_FN_ATTRS
_mm256_castsi256_si128(__m256i __a)
{
return __builtin_shufflevector((__v4di)__a, (__v4di)__a, 0, 1);
}
















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_castpd128_pd256(__m128d __a)
{
return __builtin_shufflevector((__v2df)__a, (__v2df)__a, 0, 1, -1, -1);
}
















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_castps128_ps256(__m128 __a)
{
return __builtin_shufflevector((__v4sf)__a, (__v4sf)__a, 0, 1, 2, 3, -1, -1, -1, -1);
}














static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_castsi128_si256(__m128i __a)
{
return __builtin_shufflevector((__v2di)__a, (__v2di)__a, 0, 1, -1, -1);
}














static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_zextpd128_pd256(__m128d __a)
{
return __builtin_shufflevector((__v2df)__a, (__v2df)_mm_setzero_pd(), 0, 1, 2, 3);
}













static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_zextps128_ps256(__m128 __a)
{
return __builtin_shufflevector((__v4sf)__a, (__v4sf)_mm_setzero_ps(), 0, 1, 2, 3, 4, 5, 6, 7);
}













static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_zextsi128_si256(__m128i __a)
{
return __builtin_shufflevector((__v2di)__a, (__v2di)_mm_setzero_si128(), 0, 1, 2, 3);
}








































#define _mm256_insertf128_ps(V1, V2, M) (__m256)__builtin_ia32_vinsertf128_ps256((__v8sf)(__m256)(V1), (__v4sf)(__m128)(V2), (int)(M))





































#define _mm256_insertf128_pd(V1, V2, M) (__m256d)__builtin_ia32_vinsertf128_pd256((__v4df)(__m256d)(V1), (__v2df)(__m128d)(V2), (int)(M))





































#define _mm256_insertf128_si256(V1, V2, M) (__m256i)__builtin_ia32_vinsertf128_si256((__v8si)(__m256i)(V1), (__v4si)(__m128i)(V2), (int)(M))





























#define _mm256_extractf128_ps(V, M) (__m128)__builtin_ia32_vextractf128_ps256((__v8sf)(__m256)(V), (int)(M))























#define _mm256_extractf128_pd(V, M) (__m128d)__builtin_ia32_vextractf128_pd256((__v4df)(__m256d)(V), (int)(M))























#define _mm256_extractf128_si256(V, M) (__m128i)__builtin_ia32_vextractf128_si256((__v8si)(__m256i)(V), (int)(M))
























static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_loadu2_m128(float const *__addr_hi, float const *__addr_lo)
{
__m256 __v256 = _mm256_castps128_ps256(_mm_loadu_ps(__addr_lo));
return _mm256_insertf128_ps(__v256, _mm_loadu_ps(__addr_hi), 1);
}






















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_loadu2_m128d(double const *__addr_hi, double const *__addr_lo)
{
__m256d __v256 = _mm256_castpd128_pd256(_mm_loadu_pd(__addr_lo));
return _mm256_insertf128_pd(__v256, _mm_loadu_pd(__addr_hi), 1);
}



















static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_loadu2_m128i(__m128i_u const *__addr_hi, __m128i_u const *__addr_lo)
{
__m256i __v256 = _mm256_castsi128_si256(_mm_loadu_si128(__addr_lo));
return _mm256_insertf128_si256(__v256, _mm_loadu_si128(__addr_hi), 1);
}




















static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu2_m128(float *__addr_hi, float *__addr_lo, __m256 __a)
{
__m128 __v128;

__v128 = _mm256_castps256_ps128(__a);
_mm_storeu_ps(__addr_lo, __v128);
__v128 = _mm256_extractf128_ps(__a, 1);
_mm_storeu_ps(__addr_hi, __v128);
}



















static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu2_m128d(double *__addr_hi, double *__addr_lo, __m256d __a)
{
__m128d __v128;

__v128 = _mm256_castpd256_pd128(__a);
_mm_storeu_pd(__addr_lo, __v128);
__v128 = _mm256_extractf128_pd(__a, 1);
_mm_storeu_pd(__addr_hi, __v128);
}



















static __inline void __DEFAULT_FN_ATTRS
_mm256_storeu2_m128i(__m128i_u *__addr_hi, __m128i_u *__addr_lo, __m256i __a)
{
__m128i __v128;

__v128 = _mm256_castsi256_si128(__a);
_mm_storeu_si128(__addr_lo, __v128);
__v128 = _mm256_extractf128_si256(__a, 1);
_mm_storeu_si128(__addr_hi, __v128);
}
















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_set_m128 (__m128 __hi, __m128 __lo)
{
return (__m256) __builtin_shufflevector((__v4sf)__lo, (__v4sf)__hi, 0, 1, 2, 3, 4, 5, 6, 7);
}
















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_set_m128d (__m128d __hi, __m128d __lo)
{
return (__m256d) __builtin_shufflevector((__v2df)__lo, (__v2df)__hi, 0, 1, 2, 3);
}















static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_set_m128i (__m128i __hi, __m128i __lo)
{
return (__m256i) __builtin_shufflevector((__v2di)__lo, (__v2di)__hi, 0, 1, 2, 3);
}


















static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_setr_m128 (__m128 __lo, __m128 __hi)
{
return _mm256_set_m128(__hi, __lo);
}


















static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_setr_m128d (__m128d __lo, __m128d __hi)
{
return (__m256d)_mm256_set_m128d(__hi, __lo);
}
















static __inline __m256i __DEFAULT_FN_ATTRS
_mm256_setr_m128i (__m128i __lo, __m128i __hi)
{
return (__m256i)_mm256_set_m128i(__hi, __lo);
}

#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS128

#endif
