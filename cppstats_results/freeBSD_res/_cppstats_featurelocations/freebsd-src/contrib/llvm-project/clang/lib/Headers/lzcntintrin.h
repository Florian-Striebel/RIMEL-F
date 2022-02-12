








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <lzcntintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__LZCNTINTRIN_H)
#define __LZCNTINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("lzcnt")))

#if !defined(_MSC_VER)










#define __lzcnt16(X) __builtin_ia32_lzcnt_u16((unsigned short)(X))
#endif












static __inline__ unsigned int __DEFAULT_FN_ATTRS
__lzcnt32(unsigned int __X)
{
return __builtin_ia32_lzcnt_u32(__X);
}












static __inline__ unsigned int __DEFAULT_FN_ATTRS
_lzcnt_u32(unsigned int __X)
{
return __builtin_ia32_lzcnt_u32(__X);
}

#if defined(__x86_64__)
#if !defined(_MSC_VER)











#define __lzcnt64(X) __builtin_ia32_lzcnt_u64((unsigned long long)(X))
#endif












static __inline__ unsigned long long __DEFAULT_FN_ATTRS
_lzcnt_u64(unsigned long long __X)
{
return __builtin_ia32_lzcnt_u64(__X);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
