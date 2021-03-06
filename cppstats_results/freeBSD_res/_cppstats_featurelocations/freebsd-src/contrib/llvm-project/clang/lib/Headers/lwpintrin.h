








#if !defined(__X86INTRIN_H)
#error "Never use <lwpintrin.h> directly; include <x86intrin.h> instead."
#endif

#if !defined(__LWPINTRIN_H)
#define __LWPINTRIN_H


#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("lwp")))












static __inline__ void __DEFAULT_FN_ATTRS
__llwpcb (void *__addr)
{
__builtin_ia32_llwpcb(__addr);
}










static __inline__ void* __DEFAULT_FN_ATTRS
__slwpcb (void)
{
return __builtin_ia32_slwpcb();
}


















#define __lwpins32(DATA2, DATA1, FLAGS) (__builtin_ia32_lwpins32((unsigned int) (DATA2), (unsigned int) (DATA1), (unsigned int) (FLAGS)))

















#define __lwpval32(DATA2, DATA1, FLAGS) (__builtin_ia32_lwpval32((unsigned int) (DATA2), (unsigned int) (DATA1), (unsigned int) (FLAGS)))



#if defined(__x86_64__)


















#define __lwpins64(DATA2, DATA1, FLAGS) (__builtin_ia32_lwpins64((unsigned long long) (DATA2), (unsigned int) (DATA1), (unsigned int) (FLAGS)))

















#define __lwpval64(DATA2, DATA1, FLAGS) (__builtin_ia32_lwpval64((unsigned long long) (DATA2), (unsigned int) (DATA1), (unsigned int) (FLAGS)))



#endif

#undef __DEFAULT_FN_ATTRS

#endif
