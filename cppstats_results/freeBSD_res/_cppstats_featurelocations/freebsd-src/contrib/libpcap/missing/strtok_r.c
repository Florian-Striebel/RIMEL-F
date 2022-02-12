



































#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "portability.h"

char *
pcap_strtok_r(char *s, const char *delim, char **last)
{
char *spanp, *tok;
int c, sc;

if (s == NULL && (s = *last) == NULL)
return (NULL);




cont:
c = *s++;
for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
if (c == sc)
goto cont;
}

if (c == 0) {
*last = NULL;
return (NULL);
}
tok = s - 1;





for (;;) {
c = *s++;
spanp = (char *)delim;
do {
if ((sc = *spanp++) == c) {
if (c == 0)
s = NULL;
else
s[-1] = '\0';
*last = s;
return (tok);
}
} while (sc != 0);
}

}
