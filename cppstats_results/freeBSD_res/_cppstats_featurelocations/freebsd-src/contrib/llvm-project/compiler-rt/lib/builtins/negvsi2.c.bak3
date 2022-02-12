











#include "int_lib.h"





COMPILER_RT_ABI si_int __negvsi2(si_int a) {
const si_int MIN = (si_int)1 << ((int)(sizeof(si_int) * CHAR_BIT) - 1);
if (a == MIN)
compilerrt_abort();
return -a;
}
