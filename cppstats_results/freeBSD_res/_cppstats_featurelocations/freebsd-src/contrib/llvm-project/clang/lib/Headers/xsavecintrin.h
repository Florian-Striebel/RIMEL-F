








#if !defined(__IMMINTRIN_H)
#error "Never use <xsavecintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__XSAVECINTRIN_H)
#define __XSAVECINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("xsavec")))

static __inline__ void __DEFAULT_FN_ATTRS
_xsavec(void *__p, unsigned long long __m) {
__builtin_ia32_xsavec(__p, __m);
}

#if defined(__x86_64__)
static __inline__ void __DEFAULT_FN_ATTRS
_xsavec64(void *__p, unsigned long long __m) {
__builtin_ia32_xsavec64(__p, __m);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
