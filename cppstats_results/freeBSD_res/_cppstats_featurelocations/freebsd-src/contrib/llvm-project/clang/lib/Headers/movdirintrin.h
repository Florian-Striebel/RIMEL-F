







#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <movdirintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(_MOVDIRINTRIN_H)
#define _MOVDIRINTRIN_H


static __inline__ void
__attribute__((__always_inline__, __nodebug__, __target__("movdiri")))
_directstoreu_u32 (void *__dst, unsigned int __value)
{
__builtin_ia32_directstore_u32((unsigned int *)__dst, (unsigned int)__value);
}

#if defined(__x86_64__)


static __inline__ void
__attribute__((__always_inline__, __nodebug__, __target__("movdiri")))
_directstoreu_u64 (void *__dst, unsigned long __value)
{
__builtin_ia32_directstore_u64((unsigned long *)__dst, __value);
}

#endif







static __inline__ void
__attribute__((__always_inline__, __nodebug__, __target__("movdir64b")))
_movdir64b (void *__dst __attribute__((align_value(64))), const void *__src)
{
__builtin_ia32_movdir64b(__dst, __src);
}

#endif
