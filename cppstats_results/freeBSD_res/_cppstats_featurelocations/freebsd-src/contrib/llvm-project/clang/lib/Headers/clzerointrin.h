







#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <clzerointrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__CLZEROINTRIN_H)
#define __CLZEROINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("clzero")))










static __inline__ void __DEFAULT_FN_ATTRS
_mm_clzero (void * __line)
{
__builtin_ia32_clzero ((void *)__line);
}

#undef __DEFAULT_FN_ATTRS

#endif
