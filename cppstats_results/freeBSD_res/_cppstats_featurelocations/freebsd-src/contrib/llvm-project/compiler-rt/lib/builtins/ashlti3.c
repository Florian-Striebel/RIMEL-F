











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI ti_int __ashlti3(ti_int a, si_int b) {
const int bits_in_dword = (int)(sizeof(di_int) * CHAR_BIT);
twords input;
twords result;
input.all = a;
if (b & bits_in_dword) {
result.s.low = 0;
result.s.high = input.s.low << (b - bits_in_dword);
} else {
if (b == 0)
return a;
result.s.low = input.s.low << b;
result.s.high = (input.s.high << b) | (input.s.low >> (bits_in_dword - b));
}
return result.all;
}

#endif
