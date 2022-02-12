








#if !defined(__X86INTRIN_H) && !defined(_MM3DNOW_H_INCLUDED)
#error "Never use <prfchwintrin.h> directly; include <x86intrin.h> or <mm3dnow.h> instead."
#endif

#if !defined(__PRFCHWINTRIN_H)
#define __PRFCHWINTRIN_H












static __inline__ void __attribute__((__always_inline__, __nodebug__))
_m_prefetch(void *__P)
{
__builtin_prefetch (__P, 0, 3 );
}
















static __inline__ void __attribute__((__always_inline__, __nodebug__))
_m_prefetchw(void *__P)
{
__builtin_prefetch (__P, 1, 3 );
}

#endif
