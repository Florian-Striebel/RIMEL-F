








#if !defined(__IMMINTRIN_H)
#error "Never use <xsavesintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__XSAVESINTRIN_H)
#define __XSAVESINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("xsaves")))

static __inline__ void __DEFAULT_FN_ATTRS
_xsaves(void *__p, unsigned long long __m) {
__builtin_ia32_xsaves(__p, __m);
}

static __inline__ void __DEFAULT_FN_ATTRS
_xrstors(void *__p, unsigned long long __m) {
__builtin_ia32_xrstors(__p, __m);
}

#if defined(__x86_64__)
static __inline__ void __DEFAULT_FN_ATTRS
_xrstors64(void *__p, unsigned long long __m) {
__builtin_ia32_xrstors64(__p, __m);
}

static __inline__ void __DEFAULT_FN_ATTRS
_xsaves64(void *__p, unsigned long long __m) {
__builtin_ia32_xsaves64(__p, __m);
}
#endif

#undef __DEFAULT_FN_ATTRS

#endif
