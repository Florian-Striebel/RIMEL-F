







#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <waitpkgintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__WAITPKGINTRIN_H)
#define __WAITPKGINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("waitpkg")))


static __inline__ void __DEFAULT_FN_ATTRS
_umonitor (void * __address)
{
__builtin_ia32_umonitor (__address);
}

static __inline__ unsigned char __DEFAULT_FN_ATTRS
_umwait (unsigned int __control, unsigned long long __counter)
{
return __builtin_ia32_umwait (__control,
(unsigned int)(__counter >> 32), (unsigned int)__counter);
}

static __inline__ unsigned char __DEFAULT_FN_ATTRS
_tpause (unsigned int __control, unsigned long long __counter)
{
return __builtin_ia32_tpause (__control,
(unsigned int)(__counter >> 32), (unsigned int)__counter);
}

#undef __DEFAULT_FN_ATTRS

#endif
