











#include "int_lib.h"





COMPILER_RT_ABI di_int __absvdi2(di_int a) {
const int N = (int)(sizeof(di_int) * CHAR_BIT);
if (a == ((di_int)1 << (N - 1)))
compilerrt_abort();
const di_int t = a >> (N - 1);
return (a ^ t) - t;
}
