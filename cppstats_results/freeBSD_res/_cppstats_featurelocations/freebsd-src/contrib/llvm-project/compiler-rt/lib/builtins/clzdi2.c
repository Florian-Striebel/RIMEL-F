











#include "int_lib.h"



#if !defined(__clang__) && ((defined(__sparc__) && defined(__arch64__)) || defined(__mips64) || (defined(__riscv) && __SIZEOF_POINTER__ >= 8))





#define __builtin_clz(a) __clzsi2(a)
extern int __clzsi2(si_int);
#endif



COMPILER_RT_ABI int __clzdi2(di_int a) {
dwords x;
x.all = a;
const si_int f = -(x.s.high == 0);
return clzsi((x.s.high & ~f) | (x.s.low & f)) +
(f & ((si_int)(sizeof(si_int) * CHAR_BIT)));
}
