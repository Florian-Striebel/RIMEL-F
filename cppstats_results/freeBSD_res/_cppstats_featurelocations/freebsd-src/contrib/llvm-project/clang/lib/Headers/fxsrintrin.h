








#if !defined(__IMMINTRIN_H)
#error "Never use <fxsrintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__FXSRINTRIN_H)
#define __FXSRINTRIN_H

#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("fxsr")))











static __inline__ void __DEFAULT_FN_ATTRS
_fxsave(void *__p)
{
__builtin_ia32_fxsave(__p);
}













static __inline__ void __DEFAULT_FN_ATTRS
_fxrstor(void *__p)
{
__builtin_ia32_fxrstor(__p);
}

#if defined(__x86_64__)










static __inline__ void __DEFAULT_FN_ATTRS
_fxsave64(void *__p)
{
__builtin_ia32_fxsave64(__p);
}













static __inline__ void __DEFAULT_FN_ATTRS
_fxrstor64(void *__p)
{
__builtin_ia32_fxrstor64(__p);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
