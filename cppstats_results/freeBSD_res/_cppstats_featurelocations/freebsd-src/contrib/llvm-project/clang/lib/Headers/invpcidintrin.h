








#if !defined(__IMMINTRIN_H)
#error "Never use <invpcidintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__INVPCIDINTRIN_H)
#define __INVPCIDINTRIN_H

static __inline__ void
__attribute__((__always_inline__, __nodebug__, __target__("invpcid")))
_invpcid(unsigned int __type, void *__descriptor) {
__builtin_ia32_invpcid(__type, __descriptor);
}

#endif
