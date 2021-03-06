








#if !defined(__EMMINTRIN_H)
#define __EMMINTRIN_H

#include <xmmintrin.h>

typedef double __m128d __attribute__((__vector_size__(16), __aligned__(16)));
typedef long long __m128i __attribute__((__vector_size__(16), __aligned__(16)));

typedef double __m128d_u __attribute__((__vector_size__(16), __aligned__(1)));
typedef long long __m128i_u __attribute__((__vector_size__(16), __aligned__(1)));


typedef double __v2df __attribute__ ((__vector_size__ (16)));
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
typedef short __v8hi __attribute__((__vector_size__(16)));
typedef char __v16qi __attribute__((__vector_size__(16)));


typedef unsigned long long __v2du __attribute__ ((__vector_size__ (16)));
typedef unsigned short __v8hu __attribute__((__vector_size__(16)));
typedef unsigned char __v16qu __attribute__((__vector_size__(16)));



typedef signed char __v16qs __attribute__((__vector_size__(16)));


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("sse2"), __min_vector_width__(128)))
#define __DEFAULT_FN_ATTRS_MMX __attribute__((__always_inline__, __nodebug__, __target__("mmx,sse2"), __min_vector_width__(64)))
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_add_sd(__m128d __a, __m128d __b)
{
__a[0] += __b[0];
return __a;
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_add_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2df)__a + (__v2df)__b);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_sub_sd(__m128d __a, __m128d __b)
{
__a[0] -= __b[0];
return __a;
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_sub_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2df)__a - (__v2df)__b);
}

















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_mul_sd(__m128d __a, __m128d __b)
{
__a[0] *= __b[0];
return __a;
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_mul_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2df)__a * (__v2df)__b);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_div_sd(__m128d __a, __m128d __b)
{
__a[0] /= __b[0];
return __a;
}














static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_div_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2df)__a / (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_sqrt_sd(__m128d __a, __m128d __b)
{
__m128d __c = __builtin_ia32_sqrtsd((__v2df)__b);
return __extension__ (__m128d) { __c[0], __a[1] };
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_sqrt_pd(__m128d __a)
{
return __builtin_ia32_sqrtpd((__v2df)__a);
}



















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_min_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_minsd((__v2df)__a, (__v2df)__b);
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_min_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_minpd((__v2df)__a, (__v2df)__b);
}



















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_max_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_maxsd((__v2df)__a, (__v2df)__b);
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_max_pd(__m128d __a, __m128d __b)
{
return __builtin_ia32_maxpd((__v2df)__a, (__v2df)__b);
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_and_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2du)__a & (__v2du)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_andnot_pd(__m128d __a, __m128d __b)
{
return (__m128d)(~(__v2du)__a & (__v2du)__b);
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_or_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2du)__a | (__v2du)__b);
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_xor_pd(__m128d __a, __m128d __b)
{
return (__m128d)((__v2du)__a ^ (__v2du)__b);
}














static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpeq_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpeqpd((__v2df)__a, (__v2df)__b);
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmplt_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpltpd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmple_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmplepd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpgt_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpltpd((__v2df)__b, (__v2df)__a);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpge_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmplepd((__v2df)__b, (__v2df)__a);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpord_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpordpd((__v2df)__a, (__v2df)__b);
}



















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpunord_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpunordpd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpneq_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpneqpd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnlt_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnltpd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnle_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnlepd((__v2df)__a, (__v2df)__b);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpngt_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnltpd((__v2df)__b, (__v2df)__a);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnge_pd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnlepd((__v2df)__b, (__v2df)__a);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpeq_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpeqsd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmplt_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpltsd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmple_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmplesd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpgt_sd(__m128d __a, __m128d __b)
{
__m128d __c = __builtin_ia32_cmpltsd((__v2df)__b, (__v2df)__a);
return __extension__ (__m128d) { __c[0], __a[1] };
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpge_sd(__m128d __a, __m128d __b)
{
__m128d __c = __builtin_ia32_cmplesd((__v2df)__b, (__v2df)__a);
return __extension__ (__m128d) { __c[0], __a[1] };
}






















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpord_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpordsd((__v2df)__a, (__v2df)__b);
}























static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpunord_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpunordsd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpneq_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpneqsd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnlt_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnltsd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnle_sd(__m128d __a, __m128d __b)
{
return (__m128d)__builtin_ia32_cmpnlesd((__v2df)__a, (__v2df)__b);
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpngt_sd(__m128d __a, __m128d __b)
{
__m128d __c = __builtin_ia32_cmpnltsd((__v2df)__b, (__v2df)__a);
return __extension__ (__m128d) { __c[0], __a[1] };
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cmpnge_sd(__m128d __a, __m128d __b)
{
__m128d __c = __builtin_ia32_cmpnlesd((__v2df)__b, (__v2df)__a);
return __extension__ (__m128d) { __c[0], __a[1] };
}



















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comieq_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdeq((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comilt_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdlt((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comile_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdle((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comigt_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdgt((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comige_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdge((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_comineq_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_comisdneq((__v2df)__a, (__v2df)__b);
}



















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomieq_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdeq((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomilt_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdlt((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomile_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdle((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomigt_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdgt((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomige_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdge((__v2df)__a, (__v2df)__b);
}





















static __inline__ int __DEFAULT_FN_ATTRS
_mm_ucomineq_sd(__m128d __a, __m128d __b)
{
return __builtin_ia32_ucomisdneq((__v2df)__a, (__v2df)__b);
}














static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_cvtpd_ps(__m128d __a)
{
return __builtin_ia32_cvtpd2ps((__v2df)__a);
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cvtps_pd(__m128 __a)
{
return (__m128d) __builtin_convertvector(
__builtin_shufflevector((__v4sf)__a, (__v4sf)__a, 0, 1), __v2df);
}

















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cvtepi32_pd(__m128i __a)
{
return (__m128d) __builtin_convertvector(
__builtin_shufflevector((__v4si)__a, (__v4si)__a, 0, 1), __v2df);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtpd_epi32(__m128d __a)
{
return __builtin_ia32_cvtpd2dq((__v2df)__a);
}












static __inline__ int __DEFAULT_FN_ATTRS
_mm_cvtsd_si32(__m128d __a)
{
return __builtin_ia32_cvtsd2si((__v2df)__a);
}




















static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_cvtsd_ss(__m128 __a, __m128d __b)
{
return (__m128)__builtin_ia32_cvtsd2ss((__v4sf)__a, (__v2df)__b);
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cvtsi32_sd(__m128d __a, int __b)
{
__a[0] = __b;
return __a;
}




















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cvtss_sd(__m128d __a, __m128 __b)
{
__a[0] = __b[0];
return __a;
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvttpd_epi32(__m128d __a)
{
return (__m128i)__builtin_ia32_cvttpd2dq((__v2df)__a);
}













static __inline__ int __DEFAULT_FN_ATTRS
_mm_cvttsd_si32(__m128d __a)
{
return __builtin_ia32_cvttsd2si((__v2df)__a);
}












static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_cvtpd_pi32(__m128d __a)
{
return (__m64)__builtin_ia32_cvtpd2pi((__v2df)__a);
}















static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_cvttpd_pi32(__m128d __a)
{
return (__m64)__builtin_ia32_cvttpd2pi((__v2df)__a);
}












static __inline__ __m128d __DEFAULT_FN_ATTRS_MMX
_mm_cvtpi32_pd(__m64 __a)
{
return __builtin_ia32_cvtpi2pd((__v2si)__a);
}












static __inline__ double __DEFAULT_FN_ATTRS
_mm_cvtsd_f64(__m128d __a)
{
return __a[0];
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_load_pd(double const *__dp)
{
return *(const __m128d*)__dp;
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_load1_pd(double const *__dp)
{
struct __mm_load1_pd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
double __u = ((const struct __mm_load1_pd_struct*)__dp)->__u;
return __extension__ (__m128d){ __u, __u };
}

#define _mm_load_pd1(dp) _mm_load1_pd(dp)















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_loadr_pd(double const *__dp)
{
__m128d __u = *(const __m128d*)__dp;
return __builtin_shufflevector((__v2df)__u, (__v2df)__u, 1, 0);
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_loadu_pd(double const *__dp)
{
struct __loadu_pd {
__m128d_u __v;
} __attribute__((__packed__, __may_alias__));
return ((const struct __loadu_pd*)__dp)->__v;
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_loadu_si64(void const *__a)
{
struct __loadu_si64 {
long long __v;
} __attribute__((__packed__, __may_alias__));
long long __u = ((const struct __loadu_si64*)__a)->__v;
return __extension__ (__m128i)(__v2di){__u, 0LL};
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_loadu_si32(void const *__a)
{
struct __loadu_si32 {
int __v;
} __attribute__((__packed__, __may_alias__));
int __u = ((const struct __loadu_si32*)__a)->__v;
return __extension__ (__m128i)(__v4si){__u, 0, 0, 0};
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_loadu_si16(void const *__a)
{
struct __loadu_si16 {
short __v;
} __attribute__((__packed__, __may_alias__));
short __u = ((const struct __loadu_si16*)__a)->__v;
return __extension__ (__m128i)(__v8hi){__u, 0, 0, 0, 0, 0, 0, 0};
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_load_sd(double const *__dp)
{
struct __mm_load_sd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
double __u = ((const struct __mm_load_sd_struct*)__dp)->__u;
return __extension__ (__m128d){ __u, 0 };
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_loadh_pd(__m128d __a, double const *__dp)
{
struct __mm_loadh_pd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
double __u = ((const struct __mm_loadh_pd_struct*)__dp)->__u;
return __extension__ (__m128d){ __a[0], __u };
}


















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_loadl_pd(__m128d __a, double const *__dp)
{
struct __mm_loadl_pd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
double __u = ((const struct __mm_loadl_pd_struct*)__dp)->__u;
return __extension__ (__m128d){ __u, __a[1] };
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_undefined_pd(void)
{
return (__m128d)__builtin_ia32_undef128();
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_set_sd(double __w)
{
return __extension__ (__m128d){ __w, 0 };
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_set1_pd(double __w)
{
return __extension__ (__m128d){ __w, __w };
}













static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_set_pd1(double __w)
{
return _mm_set1_pd(__w);
}















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_set_pd(double __w, double __x)
{
return __extension__ (__m128d){ __x, __w };
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_setr_pd(double __w, double __x)
{
return __extension__ (__m128d){ __w, __x };
}










static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_setzero_pd(void)
{
return __extension__ (__m128d){ 0, 0 };
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_move_sd(__m128d __a, __m128d __b)
{
__a[0] = __b[0];
return __a;
}












static __inline__ void __DEFAULT_FN_ATTRS
_mm_store_sd(double *__dp, __m128d __a)
{
struct __mm_store_sd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
((struct __mm_store_sd_struct*)__dp)->__u = __a[0];
}














static __inline__ void __DEFAULT_FN_ATTRS
_mm_store_pd(double *__dp, __m128d __a)
{
*(__m128d*)__dp = __a;
}















static __inline__ void __DEFAULT_FN_ATTRS
_mm_store1_pd(double *__dp, __m128d __a)
{
__a = __builtin_shufflevector((__v2df)__a, (__v2df)__a, 0, 0);
_mm_store_pd(__dp, __a);
}















static __inline__ void __DEFAULT_FN_ATTRS
_mm_store_pd1(double *__dp, __m128d __a)
{
_mm_store1_pd(__dp, __a);
}













static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeu_pd(double *__dp, __m128d __a)
{
struct __storeu_pd {
__m128d_u __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_pd*)__dp)->__v = __a;
}















static __inline__ void __DEFAULT_FN_ATTRS
_mm_storer_pd(double *__dp, __m128d __a)
{
__a = __builtin_shufflevector((__v2df)__a, (__v2df)__a, 1, 0);
*(__m128d *)__dp = __a;
}












static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeh_pd(double *__dp, __m128d __a)
{
struct __mm_storeh_pd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
((struct __mm_storeh_pd_struct*)__dp)->__u = __a[1];
}












static __inline__ void __DEFAULT_FN_ATTRS
_mm_storel_pd(double *__dp, __m128d __a)
{
struct __mm_storeh_pd_struct {
double __u;
} __attribute__((__packed__, __may_alias__));
((struct __mm_storeh_pd_struct*)__dp)->__u = __a[0];
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_add_epi8(__m128i __a, __m128i __b)
{
return (__m128i)((__v16qu)__a + (__v16qu)__b);
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_add_epi16(__m128i __a, __m128i __b)
{
return (__m128i)((__v8hu)__a + (__v8hu)__b);
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_add_epi32(__m128i __a, __m128i __b)
{
return (__m128i)((__v4su)__a + (__v4su)__b);
}













static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_add_si64(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_paddq((__v1di)__a, (__v1di)__b);
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_add_epi64(__m128i __a, __m128i __b)
{
return (__m128i)((__v2du)__a + (__v2du)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_adds_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_paddsb128((__v16qi)__a, (__v16qi)__b);
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_adds_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_paddsw128((__v8hi)__a, (__v8hi)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_adds_epu8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_paddusb128((__v16qi)__a, (__v16qi)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_adds_epu16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_paddusw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_avg_epu8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pavgb128((__v16qi)__a, (__v16qi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_avg_epu16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pavgw128((__v8hi)__a, (__v8hi)__b);
}





















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_madd_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmaddwd128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmaxsw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_max_epu8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmaxub128((__v16qi)__a, (__v16qi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pminsw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_min_epu8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pminub128((__v16qi)__a, (__v16qi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mulhi_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmulhw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mulhi_epu16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_pmulhuw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mullo_epi16(__m128i __a, __m128i __b)
{
return (__m128i)((__v8hu)__a * (__v8hu)__b);
}














static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_mul_su32(__m64 __a, __m64 __b)
{
return __builtin_ia32_pmuludq((__v2si)__a, (__v2si)__b);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_mul_epu32(__m128i __a, __m128i __b)
{
return __builtin_ia32_pmuludq128((__v4si)__a, (__v4si)__b);
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sad_epu8(__m128i __a, __m128i __b)
{
return __builtin_ia32_psadbw128((__v16qi)__a, (__v16qi)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sub_epi8(__m128i __a, __m128i __b)
{
return (__m128i)((__v16qu)__a - (__v16qu)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sub_epi16(__m128i __a, __m128i __b)
{
return (__m128i)((__v8hu)__a - (__v8hu)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sub_epi32(__m128i __a, __m128i __b)
{
return (__m128i)((__v4su)__a - (__v4su)__b);
}














static __inline__ __m64 __DEFAULT_FN_ATTRS_MMX
_mm_sub_si64(__m64 __a, __m64 __b)
{
return (__m64)__builtin_ia32_psubq((__v1di)__a, (__v1di)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sub_epi64(__m128i __a, __m128i __b)
{
return (__m128i)((__v2du)__a - (__v2du)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_subs_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psubsb128((__v16qi)__a, (__v16qi)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_subs_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psubsw128((__v8hi)__a, (__v8hi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_subs_epu8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psubusb128((__v16qi)__a, (__v16qi)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_subs_epu16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_psubusw128((__v8hi)__a, (__v8hi)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_and_si128(__m128i __a, __m128i __b)
{
return (__m128i)((__v2du)__a & (__v2du)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_andnot_si128(__m128i __a, __m128i __b)
{
return (__m128i)(~(__v2du)__a & (__v2du)__b);
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_or_si128(__m128i __a, __m128i __b)
{
return (__m128i)((__v2du)__a | (__v2du)__b);
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_xor_si128(__m128i __a, __m128i __b)
{
return (__m128i)((__v2du)__a ^ (__v2du)__b);
}


















#define _mm_slli_si128(a, imm) (__m128i)__builtin_ia32_pslldqi128_byteshift((__v2di)(__m128i)(a), (int)(imm))


#define _mm_bslli_si128(a, imm) (__m128i)__builtin_ia32_pslldqi128_byteshift((__v2di)(__m128i)(a), (int)(imm))















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_slli_epi16(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_psllwi128((__v8hi)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sll_epi16(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_psllw128((__v8hi)__a, (__v8hi)__count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_slli_epi32(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_pslldi128((__v4si)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sll_epi32(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_pslld128((__v4si)__a, (__v4si)__count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_slli_epi64(__m128i __a, int __count)
{
return __builtin_ia32_psllqi128((__v2di)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sll_epi64(__m128i __a, __m128i __count)
{
return __builtin_ia32_psllq128((__v2di)__a, (__v2di)__count);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srai_epi16(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_psrawi128((__v8hi)__a, __count);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sra_epi16(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_psraw128((__v8hi)__a, (__v8hi)__count);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srai_epi32(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_psradi128((__v4si)__a, __count);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_sra_epi32(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_psrad128((__v4si)__a, (__v4si)__count);
}


















#define _mm_srli_si128(a, imm) (__m128i)__builtin_ia32_psrldqi128_byteshift((__v2di)(__m128i)(a), (int)(imm))


#define _mm_bsrli_si128(a, imm) (__m128i)__builtin_ia32_psrldqi128_byteshift((__v2di)(__m128i)(a), (int)(imm))















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srli_epi16(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_psrlwi128((__v8hi)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srl_epi16(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_psrlw128((__v8hi)__a, (__v8hi)__count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srli_epi32(__m128i __a, int __count)
{
return (__m128i)__builtin_ia32_psrldi128((__v4si)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srl_epi32(__m128i __a, __m128i __count)
{
return (__m128i)__builtin_ia32_psrld128((__v4si)__a, (__v4si)__count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srli_epi64(__m128i __a, int __count)
{
return __builtin_ia32_psrlqi128((__v2di)__a, __count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_srl_epi64(__m128i __a, __m128i __count)
{
return __builtin_ia32_psrlq128((__v2di)__a, (__v2di)__count);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpeq_epi8(__m128i __a, __m128i __b)
{
return (__m128i)((__v16qi)__a == (__v16qi)__b);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpeq_epi16(__m128i __a, __m128i __b)
{
return (__m128i)((__v8hi)__a == (__v8hi)__b);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpeq_epi32(__m128i __a, __m128i __b)
{
return (__m128i)((__v4si)__a == (__v4si)__b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpgt_epi8(__m128i __a, __m128i __b)
{


return (__m128i)((__v16qs)__a > (__v16qs)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpgt_epi16(__m128i __a, __m128i __b)
{
return (__m128i)((__v8hi)__a > (__v8hi)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmpgt_epi32(__m128i __a, __m128i __b)
{
return (__m128i)((__v4si)__a > (__v4si)__b);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmplt_epi8(__m128i __a, __m128i __b)
{
return _mm_cmpgt_epi8(__b, __a);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmplt_epi16(__m128i __a, __m128i __b)
{
return _mm_cmpgt_epi16(__b, __a);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cmplt_epi32(__m128i __a, __m128i __b)
{
return _mm_cmpgt_epi32(__b, __a);
}

#if defined(__x86_64__)

















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_cvtsi64_sd(__m128d __a, long long __b)
{
__a[0] = __b;
return __a;
}












static __inline__ long long __DEFAULT_FN_ATTRS
_mm_cvtsd_si64(__m128d __a)
{
return __builtin_ia32_cvtsd2si64((__v2df)__a);
}













static __inline__ long long __DEFAULT_FN_ATTRS
_mm_cvttsd_si64(__m128d __a)
{
return __builtin_ia32_cvttsd2si64((__v2df)__a);
}
#endif










static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_cvtepi32_ps(__m128i __a)
{
return (__m128)__builtin_convertvector((__v4si)__a, __v4sf);
}











static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtps_epi32(__m128 __a)
{
return (__m128i)__builtin_ia32_cvtps2dq((__v4sf)__a);
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvttps_epi32(__m128 __a)
{
return (__m128i)__builtin_ia32_cvttps2dq((__v4sf)__a);
}











static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtsi32_si128(int __a)
{
return __extension__ (__m128i)(__v4si){ __a, 0, 0, 0 };
}

#if defined(__x86_64__)










static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_cvtsi64_si128(long long __a)
{
return __extension__ (__m128i)(__v2di){ __a, 0 };
}
#endif












static __inline__ int __DEFAULT_FN_ATTRS
_mm_cvtsi128_si32(__m128i __a)
{
__v4si __b = (__v4si)__a;
return __b[0];
}

#if defined(__x86_64__)











static __inline__ long long __DEFAULT_FN_ATTRS
_mm_cvtsi128_si64(__m128i __a)
{
return __a[0];
}
#endif











static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_load_si128(__m128i const *__p)
{
return *__p;
}











static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_loadu_si128(__m128i_u const *__p)
{
struct __loadu_si128 {
__m128i_u __v;
} __attribute__((__packed__, __may_alias__));
return ((const struct __loadu_si128*)__p)->__v;
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_loadl_epi64(__m128i_u const *__p)
{
struct __mm_loadl_epi64_struct {
long long __u;
} __attribute__((__packed__, __may_alias__));
return __extension__ (__m128i) { ((const struct __mm_loadl_epi64_struct*)__p)->__u, 0};
}










static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_undefined_si128(void)
{
return (__m128i)__builtin_ia32_undef128();
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set_epi64x(long long __q1, long long __q0)
{
return __extension__ (__m128i)(__v2di){ __q0, __q1 };
}

















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set_epi64(__m64 __q1, __m64 __q0)
{
return _mm_set_epi64x((long long)__q1, (long long)__q0);
}























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set_epi32(int __i3, int __i2, int __i1, int __i0)
{
return __extension__ (__m128i)(__v4si){ __i0, __i1, __i2, __i3};
}



































static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set_epi16(short __w7, short __w6, short __w5, short __w4, short __w3, short __w2, short __w1, short __w0)
{
return __extension__ (__m128i)(__v8hi){ __w0, __w1, __w2, __w3, __w4, __w5, __w6, __w7 };
}











































static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set_epi8(char __b15, char __b14, char __b13, char __b12, char __b11, char __b10, char __b9, char __b8, char __b7, char __b6, char __b5, char __b4, char __b3, char __b2, char __b1, char __b0)
{
return __extension__ (__m128i)(__v16qi){ __b0, __b1, __b2, __b3, __b4, __b5, __b6, __b7, __b8, __b9, __b10, __b11, __b12, __b13, __b14, __b15 };
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set1_epi64x(long long __q)
{
return _mm_set_epi64x(__q, __q);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set1_epi64(__m64 __q)
{
return _mm_set_epi64(__q, __q);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set1_epi32(int __i)
{
return _mm_set_epi32(__i, __i, __i, __i);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set1_epi16(short __w)
{
return _mm_set_epi16(__w, __w, __w, __w, __w, __w, __w, __w);
}














static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_set1_epi8(char __b)
{
return _mm_set_epi8(__b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b, __b);
}















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_setr_epi64(__m64 __q0, __m64 __q1)
{
return _mm_set_epi64(__q1, __q0);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_setr_epi32(int __i0, int __i1, int __i2, int __i3)
{
return _mm_set_epi32(__i3, __i2, __i1, __i0);
}


























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_setr_epi16(short __w0, short __w1, short __w2, short __w3, short __w4, short __w5, short __w6, short __w7)
{
return _mm_set_epi16(__w7, __w6, __w5, __w4, __w3, __w2, __w1, __w0);
}










































static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_setr_epi8(char __b0, char __b1, char __b2, char __b3, char __b4, char __b5, char __b6, char __b7, char __b8, char __b9, char __b10, char __b11, char __b12, char __b13, char __b14, char __b15)
{
return _mm_set_epi8(__b15, __b14, __b13, __b12, __b11, __b10, __b9, __b8, __b7, __b6, __b5, __b4, __b3, __b2, __b1, __b0);
}









static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_setzero_si128(void)
{
return __extension__ (__m128i)(__v2di){ 0LL, 0LL };
}













static __inline__ void __DEFAULT_FN_ATTRS
_mm_store_si128(__m128i *__p, __m128i __b)
{
*__p = __b;
}











static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeu_si128(__m128i_u *__p, __m128i __b)
{
struct __storeu_si128 {
__m128i_u __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_si128*)__p)->__v = __b;
}













static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeu_si64(void *__p, __m128i __b)
{
struct __storeu_si64 {
long long __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_si64*)__p)->__v = ((__v2di)__b)[0];
}













static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeu_si32(void *__p, __m128i __b)
{
struct __storeu_si32 {
int __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_si32*)__p)->__v = ((__v4si)__b)[0];
}













static __inline__ void __DEFAULT_FN_ATTRS
_mm_storeu_si16(void *__p, __m128i __b)
{
struct __storeu_si16 {
short __v;
} __attribute__((__packed__, __may_alias__));
((struct __storeu_si16*)__p)->__v = ((__v8hi)__b)[0];
}






















static __inline__ void __DEFAULT_FN_ATTRS
_mm_maskmoveu_si128(__m128i __d, __m128i __n, char *__p)
{
__builtin_ia32_maskmovdqu((__v16qi)__d, (__v16qi)__n, __p);
}














static __inline__ void __DEFAULT_FN_ATTRS
_mm_storel_epi64(__m128i_u *__p, __m128i __a)
{
struct __mm_storel_epi64_struct {
long long __u;
} __attribute__((__packed__, __may_alias__));
((struct __mm_storel_epi64_struct*)__p)->__u = __a[0];
}















static __inline__ void __DEFAULT_FN_ATTRS
_mm_stream_pd(double *__p, __m128d __a)
{
__builtin_nontemporal_store((__v2df)__a, (__v2df*)__p);
}














static __inline__ void __DEFAULT_FN_ATTRS
_mm_stream_si128(__m128i *__p, __m128i __a)
{
__builtin_nontemporal_store((__v2di)__a, (__v2di*)__p);
}














static __inline__ void __attribute__((__always_inline__, __nodebug__, __target__("sse2")))
_mm_stream_si32(int *__p, int __a)
{
__builtin_ia32_movnti(__p, __a);
}

#if defined(__x86_64__)













static __inline__ void __attribute__((__always_inline__, __nodebug__, __target__("sse2")))
_mm_stream_si64(long long *__p, long long __a)
{
__builtin_ia32_movnti64(__p, __a);
}
#endif

#if defined(__cplusplus)
extern "C" {
#endif











void _mm_clflush(void const * __p);










void _mm_lfence(void);










void _mm_mfence(void);

#if defined(__cplusplus)
}
#endif























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_packs_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_packsswb128((__v8hi)__a, (__v8hi)__b);
}























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_packs_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_packssdw128((__v4si)__a, (__v4si)__b);
}























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_packus_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_ia32_packuswb128((__v8hi)__a, (__v8hi)__b);
}























#define _mm_extract_epi16(a, imm) (int)(unsigned short)__builtin_ia32_vec_ext_v8hi((__v8hi)(__m128i)(a), (int)(imm))























#define _mm_insert_epi16(a, b, imm) (__m128i)__builtin_ia32_vec_set_v8hi((__v8hi)(__m128i)(a), (int)(b), (int)(imm))















static __inline__ int __DEFAULT_FN_ATTRS
_mm_movemask_epi8(__m128i __a)
{
return __builtin_ia32_pmovmskb128((__v16qi)__a);
}





























#define _mm_shuffle_epi32(a, imm) (__m128i)__builtin_ia32_pshufd((__v4si)(__m128i)(a), (int)(imm))





























#define _mm_shufflelo_epi16(a, imm) (__m128i)__builtin_ia32_pshuflw((__v8hi)(__m128i)(a), (int)(imm))





























#define _mm_shufflehi_epi16(a, imm) (__m128i)__builtin_ia32_pshufhw((__v8hi)(__m128i)(a), (int)(imm))































static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpackhi_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v16qi)__a, (__v16qi)__b, 8, 16+8, 9, 16+9, 10, 16+10, 11, 16+11, 12, 16+12, 13, 16+13, 14, 16+14, 15, 16+15);
}






















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpackhi_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v8hi)__a, (__v8hi)__b, 4, 8+4, 5, 8+5, 6, 8+6, 7, 8+7);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpackhi_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v4si)__a, (__v4si)__b, 2, 4+2, 3, 4+3);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpackhi_epi64(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v2di)__a, (__v2di)__b, 1, 2+1);
}






























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpacklo_epi8(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v16qi)__a, (__v16qi)__b, 0, 16+0, 1, 16+1, 2, 16+2, 3, 16+3, 4, 16+4, 5, 16+5, 6, 16+6, 7, 16+7);
}























static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpacklo_epi16(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v8hi)__a, (__v8hi)__b, 0, 8+0, 1, 8+1, 2, 8+2, 3, 8+3);
}


















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpacklo_epi32(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v4si)__a, (__v4si)__b, 0, 4+0, 1, 4+1);
}
















static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_unpacklo_epi64(__m128i __a, __m128i __b)
{
return (__m128i)__builtin_shufflevector((__v2di)__a, (__v2di)__b, 0, 2+0);
}












static __inline__ __m64 __DEFAULT_FN_ATTRS
_mm_movepi64_pi64(__m128i __a)
{
return (__m64)__a[0];
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_movpi64_epi64(__m64 __a)
{
return __extension__ (__m128i)(__v2di){ (long long)__a, 0 };
}













static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_move_epi64(__m128i __a)
{
return __builtin_shufflevector((__v2di)__a, _mm_setzero_si128(), 0, 2);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_unpackhi_pd(__m128d __a, __m128d __b)
{
return __builtin_shufflevector((__v2df)__a, (__v2df)__b, 1, 2+1);
}
















static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_unpacklo_pd(__m128d __a, __m128d __b)
{
return __builtin_shufflevector((__v2df)__a, (__v2df)__b, 0, 2+0);
}














static __inline__ int __DEFAULT_FN_ATTRS
_mm_movemask_pd(__m128d __a)
{
return __builtin_ia32_movmskpd((__v2df)__a);
}


























#define _mm_shuffle_pd(a, b, i) (__m128d)__builtin_ia32_shufpd((__v2df)(__m128d)(a), (__v2df)(__m128d)(b), (int)(i))














static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_castpd_ps(__m128d __a)
{
return (__m128)__a;
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_castpd_si128(__m128d __a)
{
return (__m128i)__a;
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_castps_pd(__m128 __a)
{
return (__m128d)__a;
}












static __inline__ __m128i __DEFAULT_FN_ATTRS
_mm_castps_si128(__m128 __a)
{
return (__m128i)__a;
}












static __inline__ __m128 __DEFAULT_FN_ATTRS
_mm_castsi128_ps(__m128i __a)
{
return (__m128)__a;
}












static __inline__ __m128d __DEFAULT_FN_ATTRS
_mm_castsi128_pd(__m128i __a)
{
return (__m128d)__a;
}

#if defined(__cplusplus)
extern "C" {
#endif








void _mm_pause(void);

#if defined(__cplusplus)
}
#endif
#undef __DEFAULT_FN_ATTRS
#undef __DEFAULT_FN_ATTRS_MMX

#define _MM_SHUFFLE2(x, y) (((x) << 1) | (y))

#define _MM_DENORMALS_ZERO_ON (0x0040U)
#define _MM_DENORMALS_ZERO_OFF (0x0000U)

#define _MM_DENORMALS_ZERO_MASK (0x0040U)

#define _MM_GET_DENORMALS_ZERO_MODE() (_mm_getcsr() & _MM_DENORMALS_ZERO_MASK)
#define _MM_SET_DENORMALS_ZERO_MODE(x) (_mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (x)))

#endif
