











































#include <stdio.h>
#include "arith.h"

#if !defined(Long)
#define Long long
#endif

typedef unsigned Long Ulong;

#undef HAVE_IEEE
#if defined(IEEE_8087)
#define _0 1
#define _1 0
#define HAVE_IEEE
#endif
#if defined(IEEE_MC68k)
#define _0 0
#define _1 1
#define HAVE_IEEE
#endif

#define UL (unsigned long)

int
main(void)
{
#if defined(HAVE_IEEE)
typedef union {
float f;
double d;
Ulong L[4];
#if !defined(NO_LONG_LONG)
unsigned short u[5];
long double D;
#endif
} U;
U a, b, c;
int i;

a.L[0] = b.L[0] = 0x7f800000;
c.f = a.f - b.f;
printf("#define f_QNAN 0x%lx\n", UL c.L[0]);
a.L[_0] = b.L[_0] = 0x7ff00000;
a.L[_1] = b.L[_1] = 0;
c.d = a.d - b.d;
printf("#define d_QNAN0 0x%lx\n", UL c.L[0]);
printf("#define d_QNAN1 0x%lx\n", UL c.L[1]);
#if defined(NO_LONG_LONG)
for(i = 0; i < 4; i++)
printf("#define ld_QNAN%d 0xffffffff\n", i);
for(i = 0; i < 5; i++)
printf("#define ldus_QNAN%d 0xffff\n", i);
#else
b.D = c.D = a.d;
if (printf("") < 0)
c.D = 37;
a.L[2] = a.L[3] = 0;
a.D = b.D - c.D;
for(i = 0; i < 4; i++)
printf("#define ld_QNAN%d 0x%lx\n", i, UL a.L[i]);
for(i = 0; i < 5; i++)
printf("#define ldus_QNAN%d 0x%x\n", i, a.u[i]);
#endif
#endif
return 0;
}
