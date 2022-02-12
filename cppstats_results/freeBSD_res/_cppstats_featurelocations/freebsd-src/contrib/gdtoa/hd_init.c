






























#include "gdtoaimp.h"

unsigned char hexdig[256];

static void
#if defined(KR_headers)
htinit(h, s, inc) unsigned char *h; unsigned char *s; int inc;
#else
htinit(unsigned char *h, unsigned char *s, int inc)
#endif
{
int i, j;
for(i = 0; (j = s[i]) !=0; i++)
h[j] = i + inc;
}

void
hexdig_init_D2A(Void)
{
#define USC (unsigned char *)
htinit(hexdig, USC "0123456789", 0x10);
htinit(hexdig, USC "abcdef", 0x10 + 10);
htinit(hexdig, USC "ABCDEF", 0x10 + 10);
}
