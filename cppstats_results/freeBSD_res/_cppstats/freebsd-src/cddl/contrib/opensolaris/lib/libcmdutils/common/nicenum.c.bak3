

























#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/debug.h>
#include "libcmdutils.h"


#define INDEX_MAX (6)


CTASSERT(INDEX_MAX * 10 < sizeof (uint64_t) * 8);

void
nicenum_scale(uint64_t n, size_t units, char *buf, size_t buflen,
uint32_t flags)
{
uint64_t divamt = 1024;
uint64_t divisor = 1;
int index = 0;
int rc = 0;
char u;

if (units == 0)
units = 1;

if (n > 0) {
n *= units;
if (n < units)
goto overflow;
}

if (flags & NN_DIVISOR_1000)
divamt = 1000;










while (index < INDEX_MAX) {
uint64_t newdiv = divisor * divamt;


VERIFY3U(newdiv, >=, divamt);
VERIFY3U(newdiv, >=, divisor);

if (n < newdiv)
break;

divisor = newdiv;
index++;
}

u = " KMGTPE"[index];

if (index == 0) {
rc = snprintf(buf, buflen, "%llu", n);
} else if (n % divisor == 0) {




rc = snprintf(buf, buflen, "%llu%c", n / divisor, u);
} else {










int i;
for (i = 2; i >= 0; i--) {
if ((rc = snprintf(buf, buflen, "%.*f%c", i,
(double)n / divisor, u)) <= 5)
break;
}
}

if (rc + 1 > buflen || rc < 0)
goto overflow;

return;

overflow:

if (buflen > 10)
(void) strlcpy(buf, "<overflow>", buflen);
else
(void) strlcpy(buf, "??", buflen);
}

void
nicenum(uint64_t num, char *buf, size_t buflen)
{
nicenum_scale(num, 1, buf, buflen, 0);
}
