











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI si_int __ucmpti2(tu_int a, tu_int b) {
utwords x;
x.all = a;
utwords y;
y.all = b;
if (x.s.high < y.s.high)
return 0;
if (x.s.high > y.s.high)
return 2;
if (x.s.low < y.s.low)
return 0;
if (x.s.low > y.s.low)
return 2;
return 1;
}

#endif
