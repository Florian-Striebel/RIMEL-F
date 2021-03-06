








#if !defined(__IMMINTRIN_H)
#error "Never use <enqcmdintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__ENQCMDINTRIN_H)
#define __ENQCMDINTRIN_H


#define _DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("enqcmd")))
















static __inline__ int _DEFAULT_FN_ATTRS
_enqcmd (void *__dst, const void *__src)
{
return __builtin_ia32_enqcmd(__dst, __src);
}















static __inline__ int _DEFAULT_FN_ATTRS
_enqcmds (void *__dst, const void *__src)
{
return __builtin_ia32_enqcmds(__dst, __src);
}

#undef _DEFAULT_FN_ATTRS

#endif
