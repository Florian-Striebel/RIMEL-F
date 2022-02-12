











#include "int_lib.h"





COMPILER_RT_ABI di_int __negvdi2(di_int a) {
const di_int MIN = (di_int)1 << ((int)(sizeof(di_int) * CHAR_BIT) - 1);
if (a == MIN)
compilerrt_abort();
return -a;
}
