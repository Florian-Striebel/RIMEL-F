






























#if 0
#if !defined(lint)
static char sccsid[] = "@(#)nlist.c 8.4 (Berkeley) 4/2/94";
#endif
#endif

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/sysctl.h>

#include <stddef.h>

#include "ps.h"

fixpt_t ccpu;
int nlistread;
unsigned long mempages;
int fscale;

int
donlist(void)
{
size_t oldlen;

oldlen = sizeof(ccpu);
if (sysctlbyname("kern.ccpu", &ccpu, &oldlen, NULL, 0) == -1)
return (1);
oldlen = sizeof(fscale);
if (sysctlbyname("kern.fscale", &fscale, &oldlen, NULL, 0) == -1)
return (1);
oldlen = sizeof(mempages);
if (sysctlbyname("hw.availpages", &mempages, &oldlen, NULL, 0) == -1)
return (1);
nlistread = 1;
return (0);
}
