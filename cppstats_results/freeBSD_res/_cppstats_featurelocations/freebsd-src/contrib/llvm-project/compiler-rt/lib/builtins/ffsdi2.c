











#include "int_lib.h"




COMPILER_RT_ABI int __ffsdi2(di_int a) {
dwords x;
x.all = a;
if (x.s.low == 0) {
if (x.s.high == 0)
return 0;
return ctzsi(x.s.high) + (1 + sizeof(si_int) * CHAR_BIT);
}
return ctzsi(x.s.low) + 1;
}
