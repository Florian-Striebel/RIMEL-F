






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoIxL(s, sp, a, b) CONST char *s; char **sp; void *a; void *b;
#else
strtoIxL(CONST char *s, char **sp, void *a, void *b)
#endif
{
static FPI fpi = { 64, 1-16383-64+1, 32766 - 16383 - 64 + 1, 1, SI };
Long exp[2];
Bigint *B[2];
int k, rv[2];
ULong *L = (ULong *)a, *M = (ULong *)b;

B[0] = Balloc(1);
B[0]->wds = 2;
k = strtoIg(s, sp, &fpi, exp, B, rv);
ULtoxL(L, B[0]->x, exp[0], rv[0]);
Bfree(B[0]);
if (B[1]) {
ULtoxL(M, B[1]->x, exp[1], rv[1]);
Bfree(B[1]);
}
else {
M[0] = L[0];
M[1] = L[1];
M[2] = L[2];
}
return k;
}
