











#include "int_lib.h"




COMPILER_RT_ABI int __ffssi2(si_int a) {
if (a == 0) {
return 0;
}
return ctzsi(a) + 1;
}
