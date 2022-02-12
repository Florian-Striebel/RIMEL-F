











#include "int_lib.h"





COMPILER_RT_ABI di_int __ashldi3(di_int a, int b) {
const int bits_in_word = (int)(sizeof(si_int) * CHAR_BIT);
dwords input;
dwords result;
input.all = a;
if (b & bits_in_word) {
result.s.low = 0;
result.s.high = input.s.low << (b - bits_in_word);
} else {
if (b == 0)
return a;
result.s.low = input.s.low << b;
result.s.high = (input.s.high << b) | (input.s.low >> (bits_in_word - b));
}
return result.all;
}

#if defined(__ARM_EABI__)
COMPILER_RT_ALIAS(__ashldi3, __aeabi_llsl)
#endif
