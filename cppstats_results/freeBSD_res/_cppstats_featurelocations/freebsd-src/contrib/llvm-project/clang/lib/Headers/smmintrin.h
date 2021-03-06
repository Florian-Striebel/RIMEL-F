








#if !defined(__SMMINTRIN_H)
#define __SMMINTRIN_H

#include <tmmintrin.h>


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("sse4.1"), __min_vector_width__(128)))


#define _MM_FROUND_TO_NEAREST_INT 0x00
#define _MM_FROUND_TO_NEG_INF 0x01
#define _MM_FROUND_TO_POS_INF 0x02
#define _MM_FROUND_TO_ZERO 0x03
#define _MM_FROUND_CUR_DIRECTION 0x04

#define _MM_FROUND_RAISE_EXC 0x00
#define _MM_FROUND_NO_EXC 0x08

#define _MM_FROUND_NINT (_MM_FROUND_RAISE_EXC | _MM_FROUND_TO_NEAREST_INT)
#define _MM_FROUND_FLOOR (_MM_FROUND_RAISE_EXC | _MM_FROUND_TO_NEG_INF)
#define _MM_FROUND_CEIL (_MM_FROUND_RAISE_EXC | _MM_FROUND_TO_POS_INF)
#define _MM_FROUND_TRUNC (_MM_FROUND_RAISE_EXC | _MM_FROUND_TO_ZERO)
#define _MM_FROUND_RINT (_MM_FROUND_RAISE_EXC | _MM_FROUND_CUR_DIRECTION)
#define _MM_FROUND_NEARBYINT (_MM_FROUND_NO_EXC | _MM_FROUND_CUR_DIRECTION)
















#define _mm_ceil_ps(X) _mm_round_ps((X), _MM_FROUND_CEIL)
















#define _mm_ceil_pd(X) _mm_round_pd((X), _MM_FROUND_CEIL)
























#define _mm_ceil_ss(X, Y) _mm_round_ss((X), (Y), _MM_FROUND_CEIL)
























#define _mm_ceil_sd(X, Y) _mm_round_sd((X), (Y), _MM_FROUND_CEIL)
















#define _mm_floor_ps(X) _mm_round_ps((X), _MM_FROUND_FLOOR)
















#define _mm_floor_pd(X) _mm_round_pd((X), _MM_FROUND_FLOOR)
























#define _mm_floor_ss(X, Y) _mm_round_ss((X), (Y), _MM_FROUND_FLOOR)
























#define _mm_floor_sd(X, Y) _mm_round_sd((X), (Y), _MM_FROUND_FLOOR)































#define _mm_round_ps(X, M) (__m128)__builtin_ia32_roundps((__v4sf)(__m128)(X), (M))








































#define _mm_round_ss(X, Y, M) (__m128)__builtin_ia32_roundss((__v4sf)(__m128)(X), (__v4sf)(__m128)(Y), (M))

































#define _mm_round_pd(X, M) (__m128d)__builtin_ia32_roundpd((__v2df)(__m128d)(X), (M))








































#define _mm_round_sd(X, Y, M) (__m128d)__builtin_ia32_roundsd((__v2df)(__m128d)(X), (__v2df)(__m128d)(Y), (M))




























#define _mm_blend_pd(V1, V2, M) (__m128d) __builtin_ia32_blendpd ((__v2df)(__m128d)(V1), (__v2df)(__m128d)(V2), (int)(M))



























#define _mm_blend_ps(V1, V2, M) (__m128) __builtin_ia32_blendps ((__v4sf)(__m128)(V1), (__v4sf)(__m128)(V2), (int)(M))























static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_blendv_pd (__m128d __V1, __m128d __V2, __m128d __M)
{
return (__m128d) __builtin_ia32_blendvpd ((__v2df)__V1, (__v2df)__V2,
(__v2df)__M);
}





















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_blendv_ps (__m128 __V1, __m128 __V2, __m128 __M)
{
return (__m128) __builtin_ia32_blendvps ((__v4sf)__V1, (__v4sf)__V2,
(__v4sf)__M);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_blendv_epi8 (__m128i __V1, __m128i __V2, __m128i __M)
{
return (__m128i) __builtin_ia32_pblendvb128 ((__v16qi)__V1, (__v16qi)__V2,
(__v16qi)__M);
}

























#define _mm_blend_epi16(V1, V2, M) (__m128i) __builtin_ia32_pblendw128 ((__v8hi)(__m128i)(V1), (__v8hi)(__m128i)(V2), (int)(M))

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mullo_epi32 (__m128i __V1, __m128i __V2)
{
return (__m128i) ((__v4su)__V1 * (__v4su)__V2);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mul_epi32 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pmuldq128 ((__v4si)__V1, (__v4si)__V2);
}


































#define _mm_dp_ps(X, Y, M) (__m128) __builtin_ia32_dpps((__v4sf)(__m128)(X), (__v4sf)(__m128)(Y), (M))


































#define _mm_dp_pd(X, Y, M) (__m128d) __builtin_ia32_dppd((__v2df)(__m128d)(X), (__v2df)(__m128d)(Y), (M))
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_stream_load_si128 (__m128i const *__V)
{
return (__m128i) __builtin_nontemporal_load ((const __v2di *) __V);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epi8 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pminsb128 ((__v16qi) __V1, (__v16qi) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epi8 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pmaxsb128 ((__v16qi) __V1, (__v16qi) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epu16 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pminuw128 ((__v8hi) __V1, (__v8hi) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epu16 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pmaxuw128 ((__v8hi) __V1, (__v8hi) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epi32 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pminsd128 ((__v4si) __V1, (__v4si) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epi32 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pmaxsd128 ((__v4si) __V1, (__v4si) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epu32 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pminud128((__v4si) __V1, (__v4si) __V2);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epu32 (__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_pmaxud128((__v4si) __V1, (__v4si) __V2);
}










































#define _mm_insert_ps(X, Y, N) __builtin_ia32_insertps128((X), (Y), (N))























#define _mm_extract_ps(X, N) (__extension__ ({ union { int __i; float __f; } __t; __t.__f = __builtin_ia32_vec_ext_v4sf((__v4sf)(__m128)(X), (int)(N)); __t.__i;}))






#define _MM_EXTRACT_FLOAT(D, X, N) { (D) = __builtin_ia32_vec_ext_v4sf((__v4sf)(__m128)(X), (int)(N)); }




#define _MM_MK_INSERTPS_NDX(X, Y, Z) (((X) << 6) | ((Y) << 4) | (Z))


#define _MM_PICK_OUT_PS(X, N) _mm_insert_ps (_mm_setzero_ps(), (X), _MM_MK_INSERTPS_NDX((N), 0, 0x0e))











































#define _mm_insert_epi8(X, I, N) (__m128i)__builtin_ia32_vec_set_v16qi((__v16qi)(__m128i)(X), (int)(I), (int)(N))































#define _mm_insert_epi32(X, I, N) (__m128i)__builtin_ia32_vec_set_v4si((__v4si)(__m128i)(X), (int)(I), (int)(N))



#if defined(__x86_64__)


























#define _mm_insert_epi64(X, I, N) (__m128i)__builtin_ia32_vec_set_v2di((__v2di)(__m128i)(X), (long long)(I), (int)(N))


#endif







































#define _mm_extract_epi8(X, N) (int)(unsigned char)__builtin_ia32_vec_ext_v16qi((__v16qi)(__m128i)(X), (int)(N))

























#define _mm_extract_epi32(X, N) (int)__builtin_ia32_vec_ext_v4si((__v4si)(__m128i)(X), (int)(N))


#if defined(__x86_64__)



















#define _mm_extract_epi64(X, N) (long long)__builtin_ia32_vec_ext_v2di((__v2di)(__m128i)(X), (int)(N))

#endif














static __inline__ int __DEFAULT_FN_ATTRS
_mm_testz_si128(__m128i __M, __m128i __V)
{
return __builtin_ia32_ptestz128((__v2di)__M, (__v2di)__V);
}













static __inline__ int __DEFAULT_FN_ATTRS
_mm_testc_si128(__m128i __M, __m128i __V)
{
return __builtin_ia32_ptestc128((__v2di)__M, (__v2di)__V);
}














static __inline__ int __DEFAULT_FN_ATTRS
_mm_testnzc_si128(__m128i __M, __m128i __V)
{
return __builtin_ia32_ptestnzc128((__v2di)__M, (__v2di)__V);
}
















#define _mm_test_all_ones(V) _mm_testc_si128((V), _mm_cmpeq_epi32((V), (V)))


















#define _mm_test_mix_ones_zeros(M, V) _mm_testnzc_si128((M), (V))

















#define _mm_test_all_zeros(M, V) _mm_testz_si128 ((M), (V))














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpeq_epi64(__m128i __V1, __m128i __V2)
{
return (__m128i)((__v2di)__V1 == (__v2di)__V2);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi8_epi16(__m128i __V)
{


return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qs)__V, (__v16qs)__V, 0, 1, 2, 3, 4, 5, 6, 7), __v8hi);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi8_epi32(__m128i __V)
{


return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qs)__V, (__v16qs)__V, 0, 1, 2, 3), __v4si);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi8_epi64(__m128i __V)
{


return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qs)__V, (__v16qs)__V, 0, 1), __v2di);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi16_epi32(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v8hi)__V, (__v8hi)__V, 0, 1, 2, 3), __v4si);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi16_epi64(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v8hi)__V, (__v8hi)__V, 0, 1), __v2di);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepi32_epi64(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v4si)__V, (__v4si)__V, 0, 1), __v2di);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu8_epi16(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qu)__V, (__v16qu)__V, 0, 1, 2, 3, 4, 5, 6, 7), __v8hi);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu8_epi32(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qu)__V, (__v16qu)__V, 0, 1, 2, 3), __v4si);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu8_epi64(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v16qu)__V, (__v16qu)__V, 0, 1), __v2di);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu16_epi32(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v8hu)__V, (__v8hu)__V, 0, 1, 2, 3), __v4si);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu16_epi64(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v8hu)__V, (__v8hu)__V, 0, 1), __v2di);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtepu32_epi64(__m128i __V)
{
return (__m128i)__builtin_convertvector(__builtin_shufflevector((__v4su)__V, (__v4su)__V, 0, 1), __v2di);
}
























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_packus_epi32(__m128i __V1, __m128i __V2)
{
return (__m128i) __builtin_ia32_packusdw128((__v4si)__V1, (__v4si)__V2);
}





































#define _mm_mpsadbw_epu8(X, Y, M) (__m128i) __builtin_ia32_mpsadbw128((__v16qi)(__m128i)(X), (__v16qi)(__m128i)(Y), (M))
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_minpos_epu16(__m128i __V)
{
return (__m128i) __builtin_ia32_phminposuw128((__v8hi)__V);
}






#undef __DEFAULT_FN_ATTRS
#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("sse4.2")))


#define _SIDD_UBYTE_OPS 0x00
#define _SIDD_UWORD_OPS 0x01
#define _SIDD_SBYTE_OPS 0x02
#define _SIDD_SWORD_OPS 0x03


#define _SIDD_CMP_EQUAL_ANY 0x00
#define _SIDD_CMP_RANGES 0x04
#define _SIDD_CMP_EQUAL_EACH 0x08
#define _SIDD_CMP_EQUAL_ORDERED 0x0c


#define _SIDD_POSITIVE_POLARITY 0x00
#define _SIDD_NEGATIVE_POLARITY 0x10
#define _SIDD_MASKED_POSITIVE_POLARITY 0x20
#define _SIDD_MASKED_NEGATIVE_POLARITY 0x30


#define _SIDD_LEAST_SIGNIFICANT 0x00
#define _SIDD_MOST_SIGNIFICANT 0x40


#define _SIDD_BIT_MASK 0x00
#define _SIDD_UNIT_MASK 0x40






















































#define _mm_cmpistrm(A, B, M) (__m128i)__builtin_ia32_pcmpistrm128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))





















































#define _mm_cmpistri(A, B, M) (int)__builtin_ia32_pcmpistri128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))



























































#define _mm_cmpestrm(A, LA, B, LB, M) (__m128i)__builtin_ia32_pcmpestrm128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))


























































#define _mm_cmpestri(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestri128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))



















































#define _mm_cmpistra(A, B, M) (int)__builtin_ia32_pcmpistria128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))
















































#define _mm_cmpistrc(A, B, M) (int)__builtin_ia32_pcmpistric128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))















































#define _mm_cmpistro(A, B, M) (int)__builtin_ia32_pcmpistrio128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))

















































#define _mm_cmpistrs(A, B, M) (int)__builtin_ia32_pcmpistris128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))

















































#define _mm_cmpistrz(A, B, M) (int)__builtin_ia32_pcmpistriz128((__v16qi)(__m128i)(A), (__v16qi)(__m128i)(B), (int)(M))





















































#define _mm_cmpestra(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestria128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))





















































#define _mm_cmpestrc(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestric128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))




















































#define _mm_cmpestro(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestrio128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))






















































#define _mm_cmpestrs(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestris128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))





















































#define _mm_cmpestrz(A, LA, B, LB, M) (int)__builtin_ia32_pcmpestriz128((__v16qi)(__m128i)(A), (int)(LA), (__v16qi)(__m128i)(B), (int)(LB), (int)(M))


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpgt_epi64(__m128i __V1, __m128i __V2)
{
return (__m128i)((__v2di)__V1 > (__v2di)__V2);
}
















static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mm_crc32_u8(unsigned int __C, unsigned char __D)
{
return __builtin_ia32_crc32qi(__C, __D);
}















static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mm_crc32_u16(unsigned int __C, unsigned short __D)
{
return __builtin_ia32_crc32hi(__C, __D);
}















static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mm_crc32_u32(unsigned int __C, unsigned int __D)
{
return __builtin_ia32_crc32si(__C, __D);
}

#if defined(__x86_64__)














static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_mm_crc32_u64(unsigned long long __C, unsigned long long __D)
{
return __builtin_ia32_crc32di(__C, __D);
}
#endif

#undef __DEFAULT_FN_ATTRS

#include <popcntintrin.h>

#endif
