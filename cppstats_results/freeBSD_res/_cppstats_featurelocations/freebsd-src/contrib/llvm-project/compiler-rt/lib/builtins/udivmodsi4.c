











#include "int_lib.h"



COMPILER_RT_ABI su_int __udivmodsi4(su_int a, su_int b, su_int *rem) {
si_int d = __udivsi3(a, b);
*rem = a - (d * b);
return d;
}
