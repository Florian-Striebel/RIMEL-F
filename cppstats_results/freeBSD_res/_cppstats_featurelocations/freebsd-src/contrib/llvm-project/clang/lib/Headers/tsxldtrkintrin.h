








#if !defined(__IMMINTRIN_H)
#error "Never use <tsxldtrkintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__TSXLDTRKINTRIN_H)
#define __TSXLDTRKINTRIN_H


#define _DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("tsxldtrk")))












static __inline__ void _DEFAULT_FN_ATTRS
_xsusldtrk (void)
{
__builtin_ia32_xsusldtrk();
}












static __inline__ void _DEFAULT_FN_ATTRS
_xresldtrk (void)
{
__builtin_ia32_xresldtrk();
}

#undef _DEFAULT_FN_ATTRS

#endif
