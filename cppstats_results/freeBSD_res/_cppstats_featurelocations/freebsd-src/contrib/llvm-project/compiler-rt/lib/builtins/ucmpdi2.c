











#include "int_lib.h"





COMPILER_RT_ABI si_int __ucmpdi2(du_int a, du_int b) {
udwords x;
x.all = a;
udwords y;
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

#if defined(__ARM_EABI__)



COMPILER_RT_ABI si_int __aeabi_ulcmp(di_int a, di_int b) {
return __ucmpdi2(a, b) - 1;
}
#endif
