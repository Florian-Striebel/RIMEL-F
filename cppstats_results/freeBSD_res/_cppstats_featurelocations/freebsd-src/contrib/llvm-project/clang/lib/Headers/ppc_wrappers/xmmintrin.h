











#if !defined(NO_WARN_X86_INTRINSICS)

















#error "Please read comment above. Use -DNO_WARN_X86_INTRINSICS to disable this error."
#endif

#if !defined(_XMMINTRIN_H_INCLUDED)
#define _XMMINTRIN_H_INCLUDED

#if defined(__linux__) && defined(__ppc64__)


#define _MM_SHUFFLE(w,x,y,z) (((w) << 6) | ((x) << 4) | ((y) << 2) | (z))

#include <altivec.h>




#if defined(__STRICT_ANSI__) && (defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L))


#undef vector
#undef pixel
#undef bool
#endif


#include <mmintrin.h>


#if __STDC_HOSTED__
#include <mm_malloc.h>
#endif



typedef vector float __m128 __attribute__((__may_alias__));


typedef vector float __m128_u __attribute__((__may_alias__, __aligned__(1)));


typedef vector float __v4sf;


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_undefined_ps (void)
{
__m128 __Y = __Y;
return __Y;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_ps (void)
{
return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_ps (float const *__P)
{
return ((__m128)vec_ld(0, (__v4sf*)__P));
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_ps (float const *__P)
{
return (vec_vsx_ld(0, __P));
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadr_ps (float const *__P)
{
__v4sf __tmp;
__m128 result;
static const __vector unsigned char permute_vector =
{ 0x1C, 0x1D, 0x1E, 0x1F, 0x18, 0x19, 0x1A, 0x1B, 0x14, 0x15, 0x16,
0x17, 0x10, 0x11, 0x12, 0x13 };

__tmp = vec_ld (0, (__v4sf *) __P);
result = (__m128) vec_perm (__tmp, __tmp, permute_vector);
return result;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_ps (float __F)
{
return __extension__ (__m128)(__v4sf){ __F, __F, __F, __F };
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_ps1 (float __F)
{
return _mm_set1_ps (__F);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_ps (const float __Z, const float __Y, const float __X, const float __W)
{
return __extension__ (__m128)(__v4sf){ __W, __X, __Y, __Z };
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_ps (float __Z, float __Y, float __X, float __W)
{
return __extension__ (__m128)(__v4sf){ __Z, __Y, __X, __W };
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_ps (float *__P, __m128 __A)
{
vec_st((__v4sf)__A, 0, (__v4sf*)__P);
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_ps (float *__P, __m128 __A)
{
*(__m128_u *)__P = __A;
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storer_ps (float *__P, __m128 __A)
{
__v4sf __tmp;
static const __vector unsigned char permute_vector =
{ 0x1C, 0x1D, 0x1E, 0x1F, 0x18, 0x19, 0x1A, 0x1B, 0x14, 0x15, 0x16,
0x17, 0x10, 0x11, 0x12, 0x13 };

__tmp = (__m128) vec_perm (__A, __A, permute_vector);

_mm_store_ps (__P, __tmp);
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store1_ps (float *__P, __m128 __A)
{
__v4sf __va = vec_splat((__v4sf)__A, 0);
_mm_store_ps (__P, __va);
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_ps1 (float *__P, __m128 __A)
{
_mm_store1_ps (__P, __A);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_ss (float __F)
{
return __extension__ (__m128)(__v4sf){ __F, 0.0f, 0.0f, 0.0f };
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_move_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};

return (vec_sel ((__v4sf)__A, (__v4sf)__B, mask));
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_ss (float const *__P)
{
return _mm_set_ss (*__P);
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_ss (float *__P, __m128 __A)
{
*__P = ((__v4sf)__A)[0];
}





extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_ss (__m128 __A, __m128 __B)
{
#if defined(_ARCH_PWR7)
__m128 a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
b = vec_splat (__B, 0);
c = a + b;


return (vec_sel (__A, c, mask));
#else
__A[0] = __A[0] + __B[0];
return (__A);
#endif
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_ss (__m128 __A, __m128 __B)
{
#if defined(_ARCH_PWR7)
__m128 a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
b = vec_splat (__B, 0);
c = a - b;


return (vec_sel (__A, c, mask));
#else
__A[0] = __A[0] - __B[0];
return (__A);
#endif
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_ss (__m128 __A, __m128 __B)
{
#if defined(_ARCH_PWR7)
__m128 a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
b = vec_splat (__B, 0);
c = a * b;


return (vec_sel (__A, c, mask));
#else
__A[0] = __A[0] * __B[0];
return (__A);
#endif
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_div_ss (__m128 __A, __m128 __B)
{
#if defined(_ARCH_PWR7)
__m128 a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
b = vec_splat (__B, 0);
c = a / b;


return (vec_sel (__A, c, mask));
#else
__A[0] = __A[0] / __B[0];
return (__A);
#endif
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sqrt_ss (__m128 __A)
{
__m128 a, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
c = vec_sqrt (a);


return (vec_sel (__A, c, mask));
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_ps (__m128 __A, __m128 __B)
{
return (__m128) ((__v4sf)__A + (__v4sf)__B);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_ps (__m128 __A, __m128 __B)
{
return (__m128) ((__v4sf)__A - (__v4sf)__B);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_ps (__m128 __A, __m128 __B)
{
return (__m128) ((__v4sf)__A * (__v4sf)__B);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_div_ps (__m128 __A, __m128 __B)
{
return (__m128) ((__v4sf)__A / (__v4sf)__B);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sqrt_ps (__m128 __A)
{
return (vec_sqrt ((__v4sf)__A));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_rcp_ps (__m128 __A)
{
return (vec_re ((__v4sf)__A));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_rsqrt_ps (__m128 __A)
{
return (vec_rsqrte (__A));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_rcp_ss (__m128 __A)
{
__m128 a, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
c = _mm_rcp_ps (a);


return (vec_sel (__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_rsqrt_ss (__m128 __A)
{
__m128 a, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
c = vec_rsqrte (a);


return (vec_sel (__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_ss (__m128 __A, __m128 __B)
{
__v4sf a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat ((__v4sf)__A, 0);
b = vec_splat ((__v4sf)__B, 0);
c = vec_min (a, b);


return (vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_ss (__m128 __A, __m128 __B)
{
__v4sf a, b, c;
static const __vector unsigned int mask = {0xffffffff, 0, 0, 0};




a = vec_splat (__A, 0);
b = vec_splat (__B, 0);
c = vec_max (a, b);


return (vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_ps (__m128 __A, __m128 __B)
{
__vector __bool int m = vec_cmpgt ((__v4sf) __B, (__v4sf) __A);
return vec_sel (__B, __A, m);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_ps (__m128 __A, __m128 __B)
{
__vector __bool int m = vec_cmpgt ((__v4sf) __A, (__v4sf) __B);
return vec_sel (__B, __A, m);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_and_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_and ((__v4sf)__A, (__v4sf)__B));

}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_andnot_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_andc ((__v4sf)__B, (__v4sf)__A));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_or_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_or ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_xor_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_xor ((__v4sf)__A, (__v4sf)__B));
}




extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmpeq ((__v4sf)__A,(__v4sf) __B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmplt ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmple ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmpgt ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmpge ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_ps (__m128 __A, __m128 __B)
{
__v4sf temp = (__v4sf ) vec_cmpeq ((__v4sf) __A, (__v4sf)__B);
return ((__m128)vec_nor (temp, temp));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnlt_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmpge ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnle_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmpgt ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpngt_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmple ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnge_ps (__m128 __A, __m128 __B)
{
return ((__m128)vec_cmplt ((__v4sf)__A, (__v4sf)__B));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpord_ps (__m128 __A, __m128 __B)
{
__vector unsigned int a, b;
__vector unsigned int c, d;
static const __vector unsigned int float_exp_mask =
{ 0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000 };

a = (__vector unsigned int) vec_abs ((__v4sf)__A);
b = (__vector unsigned int) vec_abs ((__v4sf)__B);
c = (__vector unsigned int) vec_cmpgt (float_exp_mask, a);
d = (__vector unsigned int) vec_cmpgt (float_exp_mask, b);
return ((__m128 ) vec_and (c, d));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpunord_ps (__m128 __A, __m128 __B)
{
__vector unsigned int a, b;
__vector unsigned int c, d;
static const __vector unsigned int float_exp_mask =
{ 0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000 };

a = (__vector unsigned int) vec_abs ((__v4sf)__A);
b = (__vector unsigned int) vec_abs ((__v4sf)__B);
c = (__vector unsigned int) vec_cmpgt (a, float_exp_mask);
d = (__vector unsigned int) vec_cmpgt (b, float_exp_mask);
return ((__m128 ) vec_or (c, d));
}




extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpeq(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmplt(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmple(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpgt(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpge(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpeq(a, b);
c = vec_nor (c, c);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnlt_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpge(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnle_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmpgt(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpngt_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmple(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnge_ss (__m128 __A, __m128 __B)
{
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };
__v4sf a, b, c;




a = vec_splat ((__v4sf) __A, 0);
b = vec_splat ((__v4sf) __B, 0);
c = (__v4sf) vec_cmplt(a, b);


return ((__m128)vec_sel ((__v4sf)__A, c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpord_ss (__m128 __A, __m128 __B)
{
__vector unsigned int a, b;
__vector unsigned int c, d;
static const __vector unsigned int float_exp_mask =
{ 0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000 };
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };

a = (__vector unsigned int) vec_abs ((__v4sf)__A);
b = (__vector unsigned int) vec_abs ((__v4sf)__B);
c = (__vector unsigned int) vec_cmpgt (float_exp_mask, a);
d = (__vector unsigned int) vec_cmpgt (float_exp_mask, b);
c = vec_and (c, d);


return ((__m128)vec_sel ((__v4sf)__A, (__v4sf)c, mask));
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpunord_ss (__m128 __A, __m128 __B)
{
__vector unsigned int a, b;
__vector unsigned int c, d;
static const __vector unsigned int float_exp_mask =
{ 0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000 };
static const __vector unsigned int mask =
{ 0xffffffff, 0, 0, 0 };

a = (__vector unsigned int) vec_abs ((__v4sf)__A);
b = (__vector unsigned int) vec_abs ((__v4sf)__B);
c = (__vector unsigned int) vec_cmpgt (a, float_exp_mask);
d = (__vector unsigned int) vec_cmpgt (b, float_exp_mask);
c = vec_or (c, d);


return ((__m128)vec_sel ((__v4sf)__A, (__v4sf)c, mask));
}



extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comieq_ss (__m128 __A, __m128 __B)
{
return (__A[0] == __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comilt_ss (__m128 __A, __m128 __B)
{
return (__A[0] < __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comile_ss (__m128 __A, __m128 __B)
{
return (__A[0] <= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comigt_ss (__m128 __A, __m128 __B)
{
return (__A[0] > __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comige_ss (__m128 __A, __m128 __B)
{
return (__A[0] >= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comineq_ss (__m128 __A, __m128 __B)
{
return (__A[0] != __B[0]);
}









extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomieq_ss (__m128 __A, __m128 __B)
{
return (__A[0] == __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomilt_ss (__m128 __A, __m128 __B)
{
return (__A[0] < __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomile_ss (__m128 __A, __m128 __B)
{
return (__A[0] <= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomigt_ss (__m128 __A, __m128 __B)
{
return (__A[0] > __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomige_ss (__m128 __A, __m128 __B)
{
return (__A[0] >= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomineq_ss (__m128 __A, __m128 __B)
{
return (__A[0] != __B[0]);
}

extern __inline float __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtss_f32 (__m128 __A)
{
return ((__v4sf)__A)[0];
}



extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtss_si32 (__m128 __A)
{
__m64 res = 0;
#if defined(_ARCH_PWR8)
double dtmp;
__asm__(
#if defined(__LITTLE_ENDIAN__)
"xxsldwi %x0,%x0,%x0,3;\n"
#endif
"xscvspdp %x2,%x0;\n"
"fctiw %2,%2;\n"
"mfvsrd %1,%x2;\n"
: "+wa" (__A),
"=r" (res),
"=f" (dtmp)
: );
#else
res = __builtin_rint(__A[0]);
#endif
return (res);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvt_ss2si (__m128 __A)
{
return _mm_cvtss_si32 (__A);
}





extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtss_si64 (__m128 __A)
{
__m64 res = 0;
#if defined(_ARCH_PWR8)
double dtmp;
__asm__(
#if defined(__LITTLE_ENDIAN__)
"xxsldwi %x0,%x0,%x0,3;\n"
#endif
"xscvspdp %x2,%x0;\n"
"fctid %2,%2;\n"
"mfvsrd %1,%x2;\n"
: "+wa" (__A),
"=r" (res),
"=f" (dtmp)
: );
#else
res = __builtin_llrint(__A[0]);
#endif
return (res);
}


extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtss_si64x (__m128 __A)
{
return _mm_cvtss_si64 ((__v4sf) __A);
}


enum _mm_hint
{

_MM_HINT_ET0 = 7,
_MM_HINT_ET1 = 6,
_MM_HINT_T0 = 3,
_MM_HINT_T1 = 2,
_MM_HINT_T2 = 1,
_MM_HINT_NTA = 0
};



extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_prefetch (const void *__P, enum _mm_hint __I)
{

__builtin_prefetch (__P);
}



extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_pi32 (__m128 __A)
{

__v4sf temp, rounded;
__vector unsigned long long result;


temp = (__v4sf) vec_splat ((__vector long long)__A, 0);
rounded = vec_rint(temp);
result = (__vector unsigned long long) vec_cts (rounded, 0);

return (__m64) ((__vector long long) result)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvt_ps2pi (__m128 __A)
{
return _mm_cvtps_pi32 (__A);
}


extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttss_si32 (__m128 __A)
{

float temp = __A[0];

return temp;
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtt_ss2si (__m128 __A)
{
return _mm_cvttss_si32 (__A);
}


extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttss_si64 (__m128 __A)
{

float temp = __A[0];

return temp;
}


extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttss_si64x (__m128 __A)
{

float temp = __A[0];

return temp;
}



extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttps_pi32 (__m128 __A)
{
__v4sf temp;
__vector unsigned long long result;


temp = (__v4sf) vec_splat ((__vector long long)__A, 0);
result = (__vector unsigned long long) vec_cts (temp, 0);

return (__m64) ((__vector long long) result)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtt_ps2pi (__m128 __A)
{
return _mm_cvttps_pi32 (__A);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi32_ss (__m128 __A, int __B)
{
float temp = __B;
__A[0] = temp;

return __A;
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvt_si2ss (__m128 __A, int __B)
{
return _mm_cvtsi32_ss (__A, __B);
}



extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_ss (__m128 __A, long long __B)
{
float temp = __B;
__A[0] = temp;

return __A;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64x_ss (__m128 __A, long long __B)
{
return _mm_cvtsi64_ss (__A, __B);
}



extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpi32_ps (__m128 __A, __m64 __B)
{
__vector signed int vm1;
__vector float vf1;

vm1 = (__vector signed int) (__vector unsigned long long) {__B, __B};
vf1 = (__vector float) vec_ctf (vm1, 0);

return ((__m128) (__vector unsigned long long)
{ ((__vector unsigned long long)vf1) [0],
((__vector unsigned long long)__A) [1]});
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvt_pi2ps (__m128 __A, __m64 __B)
{
return _mm_cvtpi32_ps (__A, __B);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpi16_ps (__m64 __A)
{
__vector signed short vs8;
__vector signed int vi4;
__vector float vf1;

vs8 = (__vector signed short) (__vector unsigned long long) { __A, __A };
vi4 = vec_vupklsh (vs8);
vf1 = (__vector float) vec_ctf (vi4, 0);

return (__m128) vf1;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpu16_ps (__m64 __A)
{
const __vector unsigned short zero =
{ 0, 0, 0, 0, 0, 0, 0, 0 };
__vector unsigned short vs8;
__vector unsigned int vi4;
__vector float vf1;

vs8 = (__vector unsigned short) (__vector unsigned long long) { __A, __A };
vi4 = (__vector unsigned int) vec_mergel
#if defined(__LITTLE_ENDIAN__)
(vs8, zero);
#else
(zero, vs8);
#endif
vf1 = (__vector float) vec_ctf (vi4, 0);

return (__m128) vf1;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpi8_ps (__m64 __A)
{
__vector signed char vc16;
__vector signed short vs8;
__vector signed int vi4;
__vector float vf1;

vc16 = (__vector signed char) (__vector unsigned long long) { __A, __A };
vs8 = vec_vupkhsb (vc16);
vi4 = vec_vupkhsh (vs8);
vf1 = (__vector float) vec_ctf (vi4, 0);

return (__m128) vf1;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))

_mm_cvtpu8_ps (__m64 __A)
{
const __vector unsigned char zero =
{ 0, 0, 0, 0, 0, 0, 0, 0 };
__vector unsigned char vc16;
__vector unsigned short vs8;
__vector unsigned int vi4;
__vector float vf1;

vc16 = (__vector unsigned char) (__vector unsigned long long) { __A, __A };
#if defined(__LITTLE_ENDIAN__)
vs8 = (__vector unsigned short) vec_mergel (vc16, zero);
vi4 = (__vector unsigned int) vec_mergeh (vs8,
(__vector unsigned short) zero);
#else
vs8 = (__vector unsigned short) vec_mergel (zero, vc16);
vi4 = (__vector unsigned int) vec_mergeh ((__vector unsigned short) zero,
vs8);
#endif
vf1 = (__vector float) vec_ctf (vi4, 0);

return (__m128) vf1;
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpi32x2_ps (__m64 __A, __m64 __B)
{
__vector signed int vi4;
__vector float vf4;

vi4 = (__vector signed int) (__vector unsigned long long) { __A, __B };
vf4 = (__vector float) vec_ctf (vi4, 0);
return (__m128) vf4;
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_pi16 (__m128 __A)
{
__v4sf rounded;
__vector signed int temp;
__vector unsigned long long result;

rounded = vec_rint(__A);
temp = vec_cts (rounded, 0);
result = (__vector unsigned long long) vec_pack (temp, temp);

return (__m64) ((__vector long long) result)[0];
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_pi8 (__m128 __A)
{
__v4sf rounded;
__vector signed int tmp_i;
static const __vector signed int zero = {0, 0, 0, 0};
__vector signed short tmp_s;
__vector signed char res_v;

rounded = vec_rint(__A);
tmp_i = vec_cts (rounded, 0);
tmp_s = vec_pack (tmp_i, zero);
res_v = vec_pack (tmp_s, tmp_s);
return (__m64) ((__vector long long) res_v)[0];
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))

_mm_shuffle_ps (__m128 __A, __m128 __B, int const __mask)
{
unsigned long element_selector_10 = __mask & 0x03;
unsigned long element_selector_32 = (__mask >> 2) & 0x03;
unsigned long element_selector_54 = (__mask >> 4) & 0x03;
unsigned long element_selector_76 = (__mask >> 6) & 0x03;
static const unsigned int permute_selectors[4] =
{
#if defined(__LITTLE_ENDIAN__)
0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C
#else
0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F
#endif
};
__vector unsigned int t;

t[0] = permute_selectors[element_selector_10];
t[1] = permute_selectors[element_selector_32];
t[2] = permute_selectors[element_selector_54] + 0x10101010;
t[3] = permute_selectors[element_selector_76] + 0x10101010;
return vec_perm ((__v4sf) __A, (__v4sf)__B, (__vector unsigned char)t);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_ps (__m128 __A, __m128 __B)
{
return (__m128) vec_vmrglw ((__v4sf) __A, (__v4sf)__B);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_ps (__m128 __A, __m128 __B)
{
return (__m128) vec_vmrghw ((__v4sf) __A, (__v4sf)__B);
}



extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadh_pi (__m128 __A, __m64 const *__P)
{
__vector unsigned long long __a = (__vector unsigned long long)__A;
__vector unsigned long long __p = vec_splats(*__P);
__a [1] = __p [1];

return (__m128)__a;
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeh_pi (__m64 *__P, __m128 __A)
{
__vector unsigned long long __a = (__vector unsigned long long) __A;

*__P = __a[1];
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movehl_ps (__m128 __A, __m128 __B)
{
return (__m128) vec_mergel ((__vector unsigned long long)__B,
(__vector unsigned long long)__A);
}


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movelh_ps (__m128 __A, __m128 __B)
{
return (__m128) vec_mergeh ((__vector unsigned long long)__A,
(__vector unsigned long long)__B);
}



extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadl_pi (__m128 __A, __m64 const *__P)
{
__vector unsigned long long __a = (__vector unsigned long long)__A;
__vector unsigned long long __p = vec_splats(*__P);
__a [0] = __p [0];

return (__m128)__a;
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storel_pi (__m64 *__P, __m128 __A)
{
__vector unsigned long long __a = (__vector unsigned long long) __A;

*__P = __a[0];
}

#if defined(_ARCH_PWR8)



extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movemask_ps (__m128 __A)
{
__vector unsigned long long result;
static const __vector unsigned int perm_mask =
{
#if defined(__LITTLE_ENDIAN__)
0x00204060, 0x80808080, 0x80808080, 0x80808080
#else
0x80808080, 0x80808080, 0x80808080, 0x00204060
#endif
};

result = ((__vector unsigned long long)
vec_vbpermq ((__vector unsigned char) __A,
(__vector unsigned char) perm_mask));

#if defined(__LITTLE_ENDIAN__)
return result[1];
#else
return result[0];
#endif
}
#endif


extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load1_ps (float const *__P)
{
return _mm_set1_ps (*__P);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_ps1 (float const *__P)
{
return _mm_load1_ps (__P);
}


extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_extract_pi16 (__m64 const __A, int const __N)
{
unsigned int shiftr = __N & 3;
#if defined(__BIG_ENDIAN__)
shiftr = 3 - shiftr;
#endif

return ((__A >> (shiftr * 16)) & 0xffff);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pextrw (__m64 const __A, int const __N)
{
return _mm_extract_pi16 (__A, __N);
}



extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_insert_pi16 (__m64 const __A, int const __D, int const __N)
{
const int shiftl = (__N & 3) * 16;
const __m64 shiftD = (const __m64) __D << shiftl;
const __m64 mask = 0xffffUL << shiftl;
__m64 result = (__A & (~mask)) | (shiftD & mask);

return (result);
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pinsrw (__m64 const __A, int const __D, int const __N)
{
return _mm_insert_pi16 (__A, __D, __N);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))

_mm_max_pi16 (__m64 __A, __m64 __B)
{
#if _ARCH_PWR8
__vector signed short a, b, r;
__vector __bool short c;

a = (__vector signed short)vec_splats (__A);
b = (__vector signed short)vec_splats (__B);
c = (__vector __bool short)vec_cmpgt (a, b);
r = vec_sel (b, a, c);
return (__m64) ((__vector long long) r)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __A;
m2.as_m64 = __B;

res.as_short[0] =
(m1.as_short[0] > m2.as_short[0]) ? m1.as_short[0] : m2.as_short[0];
res.as_short[1] =
(m1.as_short[1] > m2.as_short[1]) ? m1.as_short[1] : m2.as_short[1];
res.as_short[2] =
(m1.as_short[2] > m2.as_short[2]) ? m1.as_short[2] : m2.as_short[2];
res.as_short[3] =
(m1.as_short[3] > m2.as_short[3]) ? m1.as_short[3] : m2.as_short[3];

return (__m64) res.as_m64;
#endif
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmaxsw (__m64 __A, __m64 __B)
{
return _mm_max_pi16 (__A, __B);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_pu8 (__m64 __A, __m64 __B)
{
#if _ARCH_PWR8
__vector unsigned char a, b, r;
__vector __bool char c;

a = (__vector unsigned char)vec_splats (__A);
b = (__vector unsigned char)vec_splats (__B);
c = (__vector __bool char)vec_cmpgt (a, b);
r = vec_sel (b, a, c);
return (__m64) ((__vector long long) r)[0];
#else
__m64_union m1, m2, res;
long i;

m1.as_m64 = __A;
m2.as_m64 = __B;


for (i = 0; i < 8; i++)
res.as_char[i] =
((unsigned char) m1.as_char[i] > (unsigned char) m2.as_char[i]) ?
m1.as_char[i] : m2.as_char[i];

return (__m64) res.as_m64;
#endif
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmaxub (__m64 __A, __m64 __B)
{
return _mm_max_pu8 (__A, __B);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_pi16 (__m64 __A, __m64 __B)
{
#if _ARCH_PWR8
__vector signed short a, b, r;
__vector __bool short c;

a = (__vector signed short)vec_splats (__A);
b = (__vector signed short)vec_splats (__B);
c = (__vector __bool short)vec_cmplt (a, b);
r = vec_sel (b, a, c);
return (__m64) ((__vector long long) r)[0];
#else
__m64_union m1, m2, res;

m1.as_m64 = __A;
m2.as_m64 = __B;

res.as_short[0] =
(m1.as_short[0] < m2.as_short[0]) ? m1.as_short[0] : m2.as_short[0];
res.as_short[1] =
(m1.as_short[1] < m2.as_short[1]) ? m1.as_short[1] : m2.as_short[1];
res.as_short[2] =
(m1.as_short[2] < m2.as_short[2]) ? m1.as_short[2] : m2.as_short[2];
res.as_short[3] =
(m1.as_short[3] < m2.as_short[3]) ? m1.as_short[3] : m2.as_short[3];

return (__m64) res.as_m64;
#endif
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pminsw (__m64 __A, __m64 __B)
{
return _mm_min_pi16 (__A, __B);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_pu8 (__m64 __A, __m64 __B)
{
#if _ARCH_PWR8
__vector unsigned char a, b, r;
__vector __bool char c;

a = (__vector unsigned char)vec_splats (__A);
b = (__vector unsigned char)vec_splats (__B);
c = (__vector __bool char)vec_cmplt (a, b);
r = vec_sel (b, a, c);
return (__m64) ((__vector long long) r)[0];
#else
__m64_union m1, m2, res;
long i;

m1.as_m64 = __A;
m2.as_m64 = __B;


for (i = 0; i < 8; i++)
res.as_char[i] =
((unsigned char) m1.as_char[i] < (unsigned char) m2.as_char[i]) ?
m1.as_char[i] : m2.as_char[i];

return (__m64) res.as_m64;
#endif
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pminub (__m64 __A, __m64 __B)
{
return _mm_min_pu8 (__A, __B);
}


extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movemask_pi8 (__m64 __A)
{
unsigned long long p =
#if defined(__LITTLE_ENDIAN__)
0x0008101820283038UL;
#else
0x3830282018100800UL;
#endif
return __builtin_bpermd (p, __A);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmovmskb (__m64 __A)
{
return _mm_movemask_pi8 (__A);
}



extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_pu16 (__m64 __A, __m64 __B)
{
__vector unsigned short a, b;
__vector unsigned short c;
__vector unsigned int w0, w1;
__vector unsigned char xform1 = {
#if defined(__LITTLE_ENDIAN__)
0x02, 0x03, 0x12, 0x13, 0x06, 0x07, 0x16, 0x17,
0x0A, 0x0B, 0x1A, 0x1B, 0x0E, 0x0F, 0x1E, 0x1F
#else
0x00, 0x01, 0x10, 0x11, 0x04, 0x05, 0x14, 0x15,
0x00, 0x01, 0x10, 0x11, 0x04, 0x05, 0x14, 0x15
#endif
};

a = (__vector unsigned short)vec_splats (__A);
b = (__vector unsigned short)vec_splats (__B);

w0 = vec_vmuleuh (a, b);
w1 = vec_vmulouh (a, b);
c = (__vector unsigned short)vec_perm (w0, w1, xform1);

return (__m64) ((__vector long long) c)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pmulhuw (__m64 __A, __m64 __B)
{
return _mm_mulhi_pu16 (__A, __B);
}



extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_pi16 (__m64 __A, int const __N)
{
unsigned long element_selector_10 = __N & 0x03;
unsigned long element_selector_32 = (__N >> 2) & 0x03;
unsigned long element_selector_54 = (__N >> 4) & 0x03;
unsigned long element_selector_76 = (__N >> 6) & 0x03;
static const unsigned short permute_selectors[4] =
{
#if defined(__LITTLE_ENDIAN__)
0x0908, 0x0B0A, 0x0D0C, 0x0F0E
#else
0x0607, 0x0405, 0x0203, 0x0001
#endif
};
__m64_union t;
__vector unsigned long long a, p, r;

#if defined(__LITTLE_ENDIAN__)
t.as_short[0] = permute_selectors[element_selector_10];
t.as_short[1] = permute_selectors[element_selector_32];
t.as_short[2] = permute_selectors[element_selector_54];
t.as_short[3] = permute_selectors[element_selector_76];
#else
t.as_short[3] = permute_selectors[element_selector_10];
t.as_short[2] = permute_selectors[element_selector_32];
t.as_short[1] = permute_selectors[element_selector_54];
t.as_short[0] = permute_selectors[element_selector_76];
#endif
p = vec_splats (t.as_m64);
a = vec_splats (__A);
r = vec_perm (a, a, (__vector unsigned char)p);
return (__m64) ((__vector long long) r)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pshufw (__m64 __A, int const __N)
{
return _mm_shuffle_pi16 (__A, __N);
}




extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskmove_si64 (__m64 __A, __m64 __N, char *__P)
{
__m64 hibit = 0x8080808080808080UL;
__m64 mask, tmp;
__m64 *p = (__m64*)__P;

tmp = *p;
mask = _mm_cmpeq_pi8 ((__N & hibit), hibit);
tmp = (tmp & (~mask)) | (__A & mask);
*p = tmp;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_maskmovq (__m64 __A, __m64 __N, char *__P)
{
_mm_maskmove_si64 (__A, __N, __P);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avg_pu8 (__m64 __A, __m64 __B)
{
__vector unsigned char a, b, c;

a = (__vector unsigned char)vec_splats (__A);
b = (__vector unsigned char)vec_splats (__B);
c = vec_avg (a, b);
return (__m64) ((__vector long long) c)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pavgb (__m64 __A, __m64 __B)
{
return _mm_avg_pu8 (__A, __B);
}


extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avg_pu16 (__m64 __A, __m64 __B)
{
__vector unsigned short a, b, c;

a = (__vector unsigned short)vec_splats (__A);
b = (__vector unsigned short)vec_splats (__B);
c = vec_avg (a, b);
return (__m64) ((__vector long long) c)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_pavgw (__m64 __A, __m64 __B)
{
return _mm_avg_pu16 (__A, __B);
}




extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sad_pu8 (__m64 __A, __m64 __B)
{
__vector unsigned char a, b;
__vector unsigned char vmin, vmax, vabsdiff;
__vector signed int vsum;
const __vector unsigned int zero =
{ 0, 0, 0, 0 };
__m64_union result = {0};

a = (__vector unsigned char) (__vector unsigned long long) { 0UL, __A };
b = (__vector unsigned char) (__vector unsigned long long) { 0UL, __B };
vmin = vec_min (a, b);
vmax = vec_max (a, b);
vabsdiff = vec_sub (vmax, vmin);

vsum = (__vector signed int) vec_sum4s (vabsdiff, zero);

vsum = vec_sums (vsum, (__vector signed int) zero);


result.as_short[0] = vsum[3];
return result.as_m64;
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_psadbw (__m64 __A, __m64 __B)
{
return _mm_sad_pu8 (__A, __B);
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_pi (__m64 *__P, __m64 __A)
{

__asm__ (
" dcbtstt 0,%0"
:
: "b" (__P)
: "memory"
);
*__P = __A;
}


extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_ps (float *__P, __m128 __A)
{

__asm__ (
" dcbtstt 0,%0"
:
: "b" (__P)
: "memory"
);
_mm_store_ps (__P, __A);
}



extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sfence (void)
{

__atomic_thread_fence (__ATOMIC_RELEASE);
}






extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_pause (void)
{


#if _ARCH_PWR8










unsigned long __PPR;

__asm__ volatile (
" mfppr %0;"
" or 31,31,31;"
" isync;"
" lwsync;"
" isync;"
" mtppr %0;"
: "=r" (__PPR)
:
: "memory"
);
#else


__atomic_thread_fence (__ATOMIC_SEQ_CST);
#endif
}


#define _MM_TRANSPOSE4_PS(row0, row1, row2, row3) do { __v4sf __r0 = (row0), __r1 = (row1), __r2 = (row2), __r3 = (row3); __v4sf __t0 = vec_vmrghw (__r0, __r1); __v4sf __t1 = vec_vmrghw (__r2, __r3); __v4sf __t2 = vec_vmrglw (__r0, __r1); __v4sf __t3 = vec_vmrglw (__r2, __r3); (row0) = (__v4sf)vec_mergeh ((__vector long long)__t0, (__vector long long)__t1); (row1) = (__v4sf)vec_mergel ((__vector long long)__t0, (__vector long long)__t1); (row2) = (__v4sf)vec_mergeh ((__vector long long)__t2, (__vector long long)__t3); (row3) = (__v4sf)vec_mergel ((__vector long long)__t2, (__vector long long)__t3); } while (0)



















#else
#include_next <xmmintrin.h>
#endif

#endif
