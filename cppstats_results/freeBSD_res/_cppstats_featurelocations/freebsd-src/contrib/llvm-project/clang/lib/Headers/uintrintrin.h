








#if !defined(__X86GPRINTRIN_H)
#error "Never use <uintrintrin.h> directly; include <x86gprintrin.h> instead."
#endif

#if !defined(__UINTRINTRIN_H)
#define __UINTRINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("uintr")))


#if defined(__x86_64__)

struct __uintr_frame
{
unsigned long long rip;
unsigned long long rflags;
unsigned long long rsp;
};
















static __inline__ void __DEFAULT_FN_ATTRS
_clui (void)
{
__builtin_ia32_clui();
}
















static __inline__ void __DEFAULT_FN_ATTRS
_stui (void)
{
__builtin_ia32_stui();
}






















static __inline__ unsigned char __DEFAULT_FN_ATTRS
_testui (void)
{
return __builtin_ia32_testui();
}

















































static __inline__ void __DEFAULT_FN_ATTRS
_senduipi (unsigned long long __a)
{
__builtin_ia32_senduipi(__a);
}

#endif

#undef __DEFAULT_FN_ATTRS

#endif
