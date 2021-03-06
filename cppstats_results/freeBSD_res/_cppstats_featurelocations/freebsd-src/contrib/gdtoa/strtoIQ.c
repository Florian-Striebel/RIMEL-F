






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtoIQ(s, sp, a, b) CONST char *s; char **sp; void *a; void *b;
#else
strtoIQ(CONST char *s, char **sp, void *a, void *b)
#endif
{
static FPI fpi = { 113, 1-16383-113+1, 32766-16383-113+1, 1, SI };
Long exp[2];
Bigint *B[2];
int k, rv[2];
ULong *L = (ULong *)a, *M = (ULong *)b;

B[0] = Balloc(2);
B[0]->wds = 4;
k = strtoIg(s, sp, &fpi, exp, B, rv);
ULtoQ(L, B[0]->x, exp[0], rv[0]);
Bfree(B[0]);
if (B[1]) {
ULtoQ(M, B[1]->x, exp[1], rv[1]);
Bfree(B[1]);
}
else {
M[0] = L[0];
M[1] = L[1];
M[2] = L[2];
M[3] = L[3];
}
return k;
}
