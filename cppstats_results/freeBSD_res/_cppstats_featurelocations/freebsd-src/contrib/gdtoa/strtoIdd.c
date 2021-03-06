






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoIdd(s, sp, f0, f1) CONST char *s; char **sp; double *f0, *f1;
#else
strtoIdd(CONST char *s, char **sp, double *f0, double *f1)
#endif
{
#if defined(Sudden_Underflow)
static FPI fpi = { 106, 1-1023, 2046-1023-106+1, 1, 1 };
#else
static FPI fpi = { 106, 1-1023-53+1, 2046-1023-106+1, 1, 0 };
#endif
Long exp[2];
Bigint *B[2];
int k, rv[2];

B[0] = Balloc(2);
B[0]->wds = 4;
k = strtoIg(s, sp, &fpi, exp, B, rv);
ULtodd((ULong*)f0, B[0]->x, exp[0], rv[0]);
Bfree(B[0]);
if (B[1]) {
ULtodd((ULong*)f1, B[1]->x, exp[1], rv[1]);
Bfree(B[1]);
}
else {
((ULong*)f1)[0] = ((ULong*)f0)[0];
((ULong*)f1)[1] = ((ULong*)f0)[1];
((ULong*)f1)[2] = ((ULong*)f0)[2];
((ULong*)f1)[3] = ((ULong*)f0)[3];
}
return k;
}
