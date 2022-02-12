











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)




COMPILER_RT_ABI int __ffsti2(ti_int a) {
twords x;
x.all = a;
if (x.s.low == 0) {
if (x.s.high == 0)
return 0;
return __builtin_ctzll(x.s.high) + (1 + sizeof(di_int) * CHAR_BIT);
}
return __builtin_ctzll(x.s.low) + 1;
}

#endif
