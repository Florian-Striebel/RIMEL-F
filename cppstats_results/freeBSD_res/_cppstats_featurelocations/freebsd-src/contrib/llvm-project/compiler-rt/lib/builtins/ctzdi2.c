











#include "int_lib.h"



#if !defined(__clang__) && ((defined(__sparc__) && defined(__arch64__)) || defined(__mips64) || (defined(__riscv) && __SIZEOF_POINTER__ >= 8))





#define __builtin_ctz(a) __ctzsi2(a)
extern int __ctzsi2(si_int);
#endif



COMPILER_RT_ABI int __ctzdi2(di_int a) {
dwords x;
x.all = a;
const si_int f = -(x.s.low == 0);
return ctzsi((x.s.high & f) | (x.s.low & ~f)) +
(f & ((si_int)(sizeof(si_int) * CHAR_BIT)));
}
