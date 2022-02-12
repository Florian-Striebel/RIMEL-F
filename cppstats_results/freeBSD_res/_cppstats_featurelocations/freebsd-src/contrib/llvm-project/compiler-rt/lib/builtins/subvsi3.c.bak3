











#include "int_lib.h"





COMPILER_RT_ABI si_int __subvsi3(si_int a, si_int b) {
si_int s = (su_int)a - (su_int)b;
if (b >= 0) {
if (s > a)
compilerrt_abort();
} else {
if (s <= a)
compilerrt_abort();
}
return s;
}
