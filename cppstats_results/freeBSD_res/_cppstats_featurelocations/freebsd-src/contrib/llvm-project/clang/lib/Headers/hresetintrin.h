







#if !defined(__X86GPRINTRIN_H)
#error "Never use <hresetintrin.h> directly; include <x86gprintrin.h> instead."
#endif

#if !defined(__HRESETINTRIN_H)
#define __HRESETINTRIN_H

#if __has_extension(gnu_asm)


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("hreset")))



















static __inline void __DEFAULT_FN_ATTRS
_hreset(int __eax)
{
__asm__ ("hreset $0" :: "a"(__eax));
}

#undef __DEFAULT_FN_ATTRS

#endif

#endif
