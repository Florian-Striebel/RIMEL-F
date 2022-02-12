








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <ptwriteintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__PTWRITEINTRIN_H)
#define __PTWRITEINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("ptwrite")))


static __inline__ void __DEFAULT_FN_ATTRS
_ptwrite32(unsigned int __value) {
__builtin_ia32_ptwrite32(__value);
}

#if defined(__x86_64__)

static __inline__ void __DEFAULT_FN_ATTRS
_ptwrite64(unsigned long long __value) {
__builtin_ia32_ptwrite64(__value);
}

#endif

#undef __DEFAULT_FN_ATTRS

#endif
