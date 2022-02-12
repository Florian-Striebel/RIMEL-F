








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <rdseedintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__RDSEEDINTRIN_H)
#define __RDSEEDINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("rdseed")))

static __inline__ int __DEFAULT_FN_ATTRS
_rdseed16_step(unsigned short *__p)
{
return __builtin_ia32_rdseed16_step(__p);
}

static __inline__ int __DEFAULT_FN_ATTRS
_rdseed32_step(unsigned int *__p)
{
return __builtin_ia32_rdseed32_step(__p);
}

#if defined(__x86_64__)
static __inline__ int __DEFAULT_FN_ATTRS
_rdseed64_step(unsigned long long *__p)
{
return __builtin_ia32_rdseed64_step(__p);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
