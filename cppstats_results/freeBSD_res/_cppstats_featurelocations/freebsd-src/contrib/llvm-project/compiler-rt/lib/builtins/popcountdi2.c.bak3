











#include "int_lib.h"



COMPILER_RT_ABI int __popcountdi2(di_int a) {
du_int x2 = (du_int)a;
x2 = x2 - ((x2 >> 1) & 0x5555555555555555uLL);

x2 = ((x2 >> 2) & 0x3333333333333333uLL) + (x2 & 0x3333333333333333uLL);

x2 = (x2 + (x2 >> 4)) & 0x0F0F0F0F0F0F0F0FuLL;

su_int x = (su_int)(x2 + (x2 >> 32));


x = x + (x >> 16);


return (x + (x >> 8)) & 0x0000007F;
}
