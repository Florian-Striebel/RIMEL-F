








#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <pconfigintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__PCONFIGINTRIN_H)
#define __PCONFIGINTRIN_H

#define __PCONFIG_KEY_PROGRAM 0x00000001

#if __has_extension(gnu_asm)


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("pconfig")))


static __inline unsigned int __DEFAULT_FN_ATTRS
_pconfig_u32(unsigned int __leaf, __SIZE_TYPE__ __d[])
{
unsigned int __result;
__asm__ ("pconfig"
: "=a" (__result), "=b" (__d[0]), "=c" (__d[1]), "=d" (__d[2])
: "a" (__leaf), "b" (__d[0]), "c" (__d[1]), "d" (__d[2])
: "cc");
return __result;
}

#undef __DEFAULT_FN_ATTRS

#endif

#endif
