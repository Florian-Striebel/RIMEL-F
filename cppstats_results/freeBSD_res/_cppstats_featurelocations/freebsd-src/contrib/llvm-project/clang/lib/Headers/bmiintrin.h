








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <bmiintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__BMIINTRIN_H)
#define __BMIINTRIN_H




#define __RELAXED_FN_ATTRS __attribute__((__always_inline__, __nodebug__))

#define _tzcnt_u16(a) (__tzcnt_u16((a)))











static __inline__ unsigned short __RELAXED_FN_ATTRS
__tzcnt_u16(unsigned short __X)
{
return __builtin_ia32_tzcnt_u16(__X);
}











static __inline__ unsigned int __RELAXED_FN_ATTRS
__tzcnt_u32(unsigned int __X)
{
return __builtin_ia32_tzcnt_u32(__X);
}











static __inline__ int __RELAXED_FN_ATTRS
_mm_tzcnt_32(unsigned int __X)
{
return __builtin_ia32_tzcnt_u32(__X);
}

#define _tzcnt_u32(a) (__tzcnt_u32((a)))

#if defined(__x86_64__)











static __inline__ unsigned long long __RELAXED_FN_ATTRS
__tzcnt_u64(unsigned long long __X)
{
return __builtin_ia32_tzcnt_u64(__X);
}











static __inline__ long long __RELAXED_FN_ATTRS
_mm_tzcnt_64(unsigned long long __X)
{
return __builtin_ia32_tzcnt_u64(__X);
}

#define _tzcnt_u64(a) (__tzcnt_u64((a)))

#endif

#undef __RELAXED_FN_ATTRS

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) || defined(__BMI__)



#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("bmi")))

#define _andn_u32(a, b) (__andn_u32((a), (b)))


#define _blsi_u32(a) (__blsi_u32((a)))

#define _blsmsk_u32(a) (__blsmsk_u32((a)))

#define _blsr_u32(a) (__blsr_u32((a)))














static __inline__ unsigned int __DEFAULT_FN_ATTRS
__andn_u32(unsigned int __X, unsigned int __Y)
{
return ~__X & __Y;
}


















static __inline__ unsigned int __DEFAULT_FN_ATTRS
__bextr_u32(unsigned int __X, unsigned int __Y)
{
return __builtin_ia32_bextr_u32(__X, __Y);
}




















static __inline__ unsigned int __DEFAULT_FN_ATTRS
_bextr_u32(unsigned int __X, unsigned int __Y, unsigned int __Z)
{
return __builtin_ia32_bextr_u32 (__X, ((__Y & 0xff) | ((__Z & 0xff) << 8)));
}


















static __inline__ unsigned int __DEFAULT_FN_ATTRS
_bextr2_u32(unsigned int __X, unsigned int __Y) {
return __builtin_ia32_bextr_u32(__X, __Y);
}












static __inline__ unsigned int __DEFAULT_FN_ATTRS
__blsi_u32(unsigned int __X)
{
return __X & -__X;
}












static __inline__ unsigned int __DEFAULT_FN_ATTRS
__blsmsk_u32(unsigned int __X)
{
return __X ^ (__X - 1);
}












static __inline__ unsigned int __DEFAULT_FN_ATTRS
__blsr_u32(unsigned int __X)
{
return __X & (__X - 1);
}

#if defined(__x86_64__)

#define _andn_u64(a, b) (__andn_u64((a), (b)))


#define _blsi_u64(a) (__blsi_u64((a)))

#define _blsmsk_u64(a) (__blsmsk_u64((a)))

#define _blsr_u64(a) (__blsr_u64((a)))














static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__andn_u64 (unsigned long long __X, unsigned long long __Y)
{
return ~__X & __Y;
}


















static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__bextr_u64(unsigned long long __X, unsigned long long __Y)
{
return __builtin_ia32_bextr_u64(__X, __Y);
}




















static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_bextr_u64(unsigned long long __X, unsigned int __Y, unsigned int __Z)
{
return __builtin_ia32_bextr_u64 (__X, ((__Y & 0xff) | ((__Z & 0xff) << 8)));
}


















static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_bextr2_u64(unsigned long long __X, unsigned long long __Y) {
return __builtin_ia32_bextr_u64(__X, __Y);
}












static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__blsi_u64(unsigned long long __X)
{
return __X & -__X;
}












static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__blsmsk_u64(unsigned long long __X)
{
return __X ^ (__X - 1);
}












static __inline__ unsigned long long __DEFAULT_FN_ATTRS
__blsr_u64(unsigned long long __X)
{
return __X & (__X - 1);
}

#endif

#undef __DEFAULT_FN_ATTRS

#endif


#endif
