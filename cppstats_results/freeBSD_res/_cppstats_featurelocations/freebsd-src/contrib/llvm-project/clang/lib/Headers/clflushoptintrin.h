








#if !defined(__IMMINTRIN_H)
#error "Never use <clflushoptintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__CLFLUSHOPTINTRIN_H)
#define __CLFLUSHOPTINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("clflushopt")))

static __inline__ void __DEFAULT_FN_ATTRS
_mm_clflushopt(void const * __m) {
__builtin_ia32_clflushopt(__m);
}

#undef __DEFAULT_FN_ATTRS

#endif
