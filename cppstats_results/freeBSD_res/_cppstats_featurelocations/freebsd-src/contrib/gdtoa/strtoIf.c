






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoIf(s, sp, f0, f1) CONST char *s; char **sp; float *f0, *f1;
#else
strtoIf(CONST char *s, char **sp, float *f0, float *f1)
#endif
{
static FPI fpi = { 24, 1-127-24+1, 254-127-24+1, 1, SI };
Long exp[2];
Bigint *B[2];
int k, rv[2];

B[0] = Balloc(0);
B[0]->wds = 1;
k = strtoIg(s, sp, &fpi, exp, B, rv);
ULtof((ULong*)f0, B[0]->x, exp[0], rv[0]);
Bfree(B[0]);
if (B[1]) {
ULtof((ULong*)f1, B[1]->x, exp[1], rv[1]);
Bfree(B[1]);
}
else
*(ULong*)f1 = *(ULong*)f0;
return k;
}
