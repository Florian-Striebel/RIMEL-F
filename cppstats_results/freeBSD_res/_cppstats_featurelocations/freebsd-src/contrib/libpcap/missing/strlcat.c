

















#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stddef.h>
#include <string.h>

#include "portability.h"








size_t
pcap_strlcat(char * restrict dst, const char * restrict src, size_t dsize)
{
const char *odst = dst;
const char *osrc = src;
size_t n = dsize;
size_t dlen;


while (n-- != 0 && *dst != '\0')
dst++;
dlen = dst - odst;
n = dsize - dlen;

if (n-- == 0)
return(dlen + strlen(src));
while (*src != '\0') {
if (n != 0) {
*dst++ = *src;
n--;
}
src++;
}
*dst = '\0';

return(dlen + (src - osrc));
}
