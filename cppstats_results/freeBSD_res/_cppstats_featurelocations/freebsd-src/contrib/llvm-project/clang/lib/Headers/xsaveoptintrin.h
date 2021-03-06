








#if !defined(__IMMINTRIN_H)
#error "Never use <xsaveoptintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__XSAVEOPTINTRIN_H)
#define __XSAVEOPTINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("xsaveopt")))

static __inline__ void __DEFAULT_FN_ATTRS
_xsaveopt(void *__p, unsigned long long __m) {
__builtin_ia32_xsaveopt(__p, __m);
}

#if defined(__x86_64__)
static __inline__ void __DEFAULT_FN_ATTRS
_xsaveopt64(void *__p, unsigned long long __m) {
__builtin_ia32_xsaveopt64(__p, __m);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
