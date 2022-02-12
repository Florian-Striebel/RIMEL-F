











#include "int_lib.h"





COMPILER_RT_ABI si_int __cmpdi2(di_int a, di_int b) {
dwords x;
x.all = a;
dwords y;
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



COMPILER_RT_ABI si_int __aeabi_lcmp(di_int a, di_int b) {
return __cmpdi2(a, b) - 1;
}
#endif
