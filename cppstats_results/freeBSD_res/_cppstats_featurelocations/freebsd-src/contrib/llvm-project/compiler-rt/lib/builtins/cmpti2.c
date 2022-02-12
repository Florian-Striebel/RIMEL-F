











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI si_int __cmpti2(ti_int a, ti_int b) {
twords x;
x.all = a;
twords y;
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
