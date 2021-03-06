








#if !defined(__X86INTRIN_H)
#error "Never use <mwaitxintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__MWAITXINTRIN_H)
#define __MWAITXINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("mwaitx")))
static __inline__ void __DEFAULT_FN_ATTRS
_mm_monitorx(void * __p, unsigned __extensions, unsigned __hints)
{
__builtin_ia32_monitorx(__p, __extensions, __hints);
}

static __inline__ void __DEFAULT_FN_ATTRS
_mm_mwaitx(unsigned __extensions, unsigned __hints, unsigned __clock)
{
__builtin_ia32_mwaitx(__extensions, __hints, __clock);
}

#undef __DEFAULT_FN_ATTRS

#endif
