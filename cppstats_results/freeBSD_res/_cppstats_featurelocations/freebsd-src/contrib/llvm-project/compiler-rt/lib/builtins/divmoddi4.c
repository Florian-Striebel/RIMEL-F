











#include "int_lib.h"



COMPILER_RT_ABI di_int __divmoddi4(di_int a, di_int b, di_int *rem) {
const int bits_in_dword_m1 = (int)(sizeof(di_int) * CHAR_BIT) - 1;
di_int s_a = a >> bits_in_dword_m1;
di_int s_b = b >> bits_in_dword_m1;
a = (a ^ s_a) - s_a;
b = (b ^ s_b) - s_b;
s_b ^= s_a;
du_int r;
di_int q = (__udivmoddi4(a, b, &r) ^ s_b) - s_b;
*rem = (r ^ s_a) - s_a;
return q;
}
