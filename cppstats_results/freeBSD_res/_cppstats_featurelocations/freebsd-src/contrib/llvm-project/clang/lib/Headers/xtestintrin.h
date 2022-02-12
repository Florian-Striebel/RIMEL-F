








#if !defined(__IMMINTRIN_H)
#error "Never use <xtestintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__XTESTINTRIN_H)
#define __XTESTINTRIN_H





static __inline__ int
__attribute__((__always_inline__, __nodebug__, __target__("rtm")))
_xtest(void) {
return __builtin_ia32_xtest();
}

#endif
