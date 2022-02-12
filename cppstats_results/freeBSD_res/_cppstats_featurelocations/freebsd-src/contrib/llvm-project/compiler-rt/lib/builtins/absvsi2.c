











#include "int_lib.h"





COMPILER_RT_ABI si_int __absvsi2(si_int a) {
const int N = (int)(sizeof(si_int) * CHAR_BIT);
if (a == ((si_int)1 << (N - 1)))
compilerrt_abort();
const si_int t = a >> (N - 1);
return (a ^ t) - t;
}
