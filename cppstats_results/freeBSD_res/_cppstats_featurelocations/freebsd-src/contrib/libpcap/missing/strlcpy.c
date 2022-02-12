

















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stddef.h>
#include <string.h>

#include "portability.h"






size_t
pcap_strlcpy(char * restrict dst, const char * restrict src, size_t dsize)
{
const char *osrc = src;
size_t nleft = dsize;


if (nleft != 0) {
while (--nleft != 0) {
if ((*dst++ = *src++) == '\0')
break;
}
}


if (nleft == 0) {
if (dsize != 0)
*dst = '\0';
while (*src++)
;
}

return(src - osrc - 1);
}
