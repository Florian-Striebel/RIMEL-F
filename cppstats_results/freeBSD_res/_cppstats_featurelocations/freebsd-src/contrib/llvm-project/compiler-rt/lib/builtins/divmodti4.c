











#include "int_lib.h"

#if defined(CRT_HAS_128BIT)



COMPILER_RT_ABI ti_int __divmodti4(ti_int a, ti_int b, ti_int *rem) {
const int bits_in_tword_m1 = (int)(sizeof(ti_int) * CHAR_BIT) - 1;
ti_int s_a = a >> bits_in_tword_m1;
ti_int s_b = b >> bits_in_tword_m1;
a = (a ^ s_a) - s_a;
b = (b ^ s_b) - s_b;
s_b ^= s_a;
tu_int r;
ti_int q = (__udivmodti4(a, b, &r) ^ s_b) - s_b;
*rem = (r ^ s_a) - s_a;
return q;
}

#endif
