











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI ti_int __negvti2(ti_int a) {
const ti_int MIN = (ti_int)1 << ((int)(sizeof(ti_int) * CHAR_BIT) - 1);
if (a == MIN)
compilerrt_abort();
return -a;
}

#endif
