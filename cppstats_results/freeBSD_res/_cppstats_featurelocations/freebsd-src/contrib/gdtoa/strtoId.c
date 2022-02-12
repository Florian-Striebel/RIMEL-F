






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoId(s, sp, f0, f1) CONST char *s; char **sp; double *f0, *f1;
#else
strtoId(CONST char *s, char **sp, double *f0, double *f1)
#endif
{
static FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
Long exp[2];
Bigint *B[2];
int k, rv[2];

B[0] = Balloc(1);
B[0]->wds = 2;
k = strtoIg(s, sp, &fpi, exp, B, rv);
ULtod((ULong*)f0, B[0]->x, exp[0], rv[0]);
Bfree(B[0]);
if (B[1]) {
ULtod((ULong*)f1, B[1]->x, exp[1], rv[1]);
Bfree(B[1]);
}
else {
((ULong*)f1)[0] = ((ULong*)f0)[0];
((ULong*)f1)[1] = ((ULong*)f0)[1];
}
return k;
}
