








#if !defined(__IMMINTRIN_H)
#error "Never use <serializeintrin.h> directly; include <immintrin.h> instead."
#endif

#if !defined(__SERIALIZEINTRIN_H)
#define __SERIALIZEINTRIN_H







static __inline__ void
__attribute__((__always_inline__, __nodebug__, __target__("serialize")))
_serialize (void)
{
__builtin_ia32_serialize ();
}

#endif
