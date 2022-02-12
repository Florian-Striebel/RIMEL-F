











#include "int_lib.h"





COMPILER_RT_ABI di_int __lshrdi3(di_int a, int b) {
const int bits_in_word = (int)(sizeof(si_int) * CHAR_BIT);
udwords input;
udwords result;
input.all = a;
if (b & bits_in_word) {
result.s.high = 0;
result.s.low = input.s.high >> (b - bits_in_word);
} else {
if (b == 0)
return a;
result.s.high = input.s.high >> b;
result.s.low = (input.s.high << (bits_in_word - b)) | (input.s.low >> b);
}
return result.all;
}

#if defined(__ARM_EABI__)
COMPILER_RT_ALIAS(__lshrdi3, __aeabi_llsr)
#endif
