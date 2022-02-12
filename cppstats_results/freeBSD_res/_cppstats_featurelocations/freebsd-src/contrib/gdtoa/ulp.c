






























#include "gdtoaimp.h"

double
ulp
#if defined(KR_headers)
(x) U *x;
#else
(U *x)
#endif
{
Long L;
U a;

L = (word0(x) & Exp_mask) - (P-1)*Exp_msk1;
#if !defined(Sudden_Underflow)
if (L > 0) {
#endif
#if defined(IBM)
L |= Exp_msk1 >> 4;
#endif
word0(&a) = L;
word1(&a) = 0;
#if !defined(Sudden_Underflow)
}
else {
L = -L >> Exp_shift;
if (L < Exp_shift) {
word0(&a) = 0x80000 >> L;
word1(&a) = 0;
}
else {
word0(&a) = 0;
L -= Exp_shift;
word1(&a) = L >= 31 ? 1 : 1 << (31 - L);
}
}
#endif
return dval(&a);
}
