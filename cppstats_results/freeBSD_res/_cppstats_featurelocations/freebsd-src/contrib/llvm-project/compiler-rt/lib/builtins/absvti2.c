











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI ti_int __absvti2(ti_int a) {
const int N = (int)(sizeof(ti_int) * CHAR_BIT);
if (a == ((ti_int)1 << (N - 1)))
compilerrt_abort();
const ti_int s = a >> (N - 1);
return (a ^ s) - s;
}

#endif
