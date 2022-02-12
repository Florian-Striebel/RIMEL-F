








#if !defined(__IMMINTRIN_H)
#error "Never use <clwbintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__CLWBINTRIN_H)
#define __CLWBINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("clwb")))












static __inline__ void __DEFAULT_FN_ATTRS
_mm_clwb(void const *__p) {
__builtin_ia32_clwb(__p);
}

#undef __DEFAULT_FN_ATTRS

#endif
