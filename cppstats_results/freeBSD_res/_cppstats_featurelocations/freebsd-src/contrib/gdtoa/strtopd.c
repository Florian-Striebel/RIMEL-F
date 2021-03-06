






























#include "gdtoaimp.h"

int
#if defined(KR_headers)
strtopd(s, sp, d) char *s; char **sp; double *d;
#else
strtopd(CONST char *s, char **sp, double *d)
#endif
{
static FPI fpi0 = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
ULong bits[2];
Long exp;
int k;
#if defined(Honor_FLT_ROUNDS)
#include "gdtoa_fltrnds.h"
#else
#define fpi &fpi0
#endif

k = strtodg(s, sp, fpi, &exp, bits);
ULtod((ULong*)d, bits, exp, k);
return k;
}
