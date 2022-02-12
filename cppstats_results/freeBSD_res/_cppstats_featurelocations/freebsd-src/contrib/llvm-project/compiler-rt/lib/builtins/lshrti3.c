











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)





COMPILER_RT_ABI ti_int __lshrti3(ti_int a, si_int b) {
const int bits_in_dword = (int)(sizeof(di_int) * CHAR_BIT);
utwords input;
utwords result;
input.all = a;
if (b & bits_in_dword) {
result.s.high = 0;
result.s.low = input.s.high >> (b - bits_in_dword);
} else {
if (b == 0)
return a;
result.s.high = input.s.high >> b;
result.s.low = (input.s.high << (bits_in_dword - b)) | (input.s.low >> b);
}
return result.all;
}

#endif
