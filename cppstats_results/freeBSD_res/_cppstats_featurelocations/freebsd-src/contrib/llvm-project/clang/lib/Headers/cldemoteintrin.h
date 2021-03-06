








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <cldemoteintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__CLDEMOTEINTRIN_H)
#define __CLDEMOTEINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("cldemote")))









static __inline__ void __DEFAULT_FN_ATTRS
_cldemote(const void * __P) {
__builtin_ia32_cldemote(__P);
}

#define _mm_cldemote(p) _cldemote(p)
#undef __DEFAULT_FN_ATTRS

#endif
