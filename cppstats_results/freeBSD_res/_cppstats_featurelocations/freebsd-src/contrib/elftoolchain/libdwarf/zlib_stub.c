








#include <sys/cdefs.h>








extern int uncompress(void *, unsigned long *, const void *,
unsigned long);

int __weak_symbol
uncompress(void *dst __unused, unsigned long *dstsz __unused,
const void *src __unused, unsigned long srcsz __unused)
{
return (-6);
}
