








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <bmi2intrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__BMI2INTRIN_H)
#define __BMI2INTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("bmi2")))

static __inline__ unsigned int __DEFAULT_FN_ATTRS
_bzhi_u32(unsigned int __X, unsigned int __Y)
{
return __builtin_ia32_bzhi_si(__X, __Y);
}

static __inline__ unsigned int __DEFAULT_FN_ATTRS
_pdep_u32(unsigned int __X, unsigned int __Y)
{
return __builtin_ia32_pdep_si(__X, __Y);
}

static __inline__ unsigned int __DEFAULT_FN_ATTRS
_pext_u32(unsigned int __X, unsigned int __Y)
{
return __builtin_ia32_pext_si(__X, __Y);
}

#if defined(__x86_64__)

static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_bzhi_u64(unsigned long long __X, unsigned long long __Y)
{
return __builtin_ia32_bzhi_di(__X, __Y);
}

static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_pdep_u64(unsigned long long __X, unsigned long long __Y)
{
return __builtin_ia32_pdep_di(__X, __Y);
}

static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_pext_u64(unsigned long long __X, unsigned long long __Y)
{
return __builtin_ia32_pext_di(__X, __Y);
}

static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_mulx_u64 (unsigned long long __X, unsigned long long __Y,
unsigned long long *__P)
{
unsigned __int128 __res = (unsigned __int128) __X * __Y;
*__P = (unsigned long long) (__res >> 64);
return (unsigned long long) __res;
}

#else

static __inline__ unsigned int __DEFAULT_FN_ATTRS
_mulx_u32 (unsigned int __X, unsigned int __Y, unsigned int *__P)
{
unsigned long long __res = (unsigned long long) __X * __Y;
*__P = (unsigned int) (__res >> 32);
return (unsigned int) __res;
}

#endif

#undef __DEFAULT_FN_ATTRS

#endif
