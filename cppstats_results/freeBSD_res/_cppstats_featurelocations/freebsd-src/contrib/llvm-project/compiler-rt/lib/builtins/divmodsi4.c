












#include "int_lib.h"



COMPILER_RT_ABI si_int __divmodsi4(si_int a, si_int b, si_int *rem) {
const int bits_in_word_m1 = (int)(sizeof(si_int) * CHAR_BIT) - 1;
si_int s_a = a >> bits_in_word_m1;
si_int s_b = b >> bits_in_word_m1;
a = (a ^ s_a) - s_a;
b = (b ^ s_b) - s_b;
s_b ^= s_a;
su_int r;
si_int q = (__udivmodsi4(a, b, &r) ^ s_b) - s_b;
*rem = (r ^ s_a) - s_a;
return q;
}
