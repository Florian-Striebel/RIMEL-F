











#include "int_lib.h"



static di_int __muldsi3(su_int a, su_int b) {
dwords r;
const int bits_in_word_2 = (int)(sizeof(si_int) * CHAR_BIT) / 2;
const su_int lower_mask = (su_int)~0 >> bits_in_word_2;
r.s.low = (a & lower_mask) * (b & lower_mask);
su_int t = r.s.low >> bits_in_word_2;
r.s.low &= lower_mask;
t += (a >> bits_in_word_2) * (b & lower_mask);
r.s.low += (t & lower_mask) << bits_in_word_2;
r.s.high = t >> bits_in_word_2;
t = r.s.low >> bits_in_word_2;
r.s.low &= lower_mask;
t += (b >> bits_in_word_2) * (a & lower_mask);
r.s.low += (t & lower_mask) << bits_in_word_2;
r.s.high += t >> bits_in_word_2;
r.s.high += (a >> bits_in_word_2) * (b >> bits_in_word_2);
return r.all;
}



COMPILER_RT_ABI di_int __muldi3(di_int a, di_int b) {
dwords x;
x.all = a;
dwords y;
y.all = b;
dwords r;
r.all = __muldsi3(x.s.low, y.s.low);
r.s.high += x.s.high * y.s.low + x.s.low * y.s.high;
return r.all;
}

#if defined(__ARM_EABI__)
COMPILER_RT_ALIAS(__muldi3, __aeabi_lmul)
#endif
