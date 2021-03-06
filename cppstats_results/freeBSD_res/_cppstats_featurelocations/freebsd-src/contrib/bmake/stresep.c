






























#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/cdefs.h>

#include <assert.h>
#include <string.h>

#if !defined(HAVE_STRESEP)
char * stresep(char **stringp, const char *delim, int esc);













char *
stresep(char **stringp, const char *delim, int esc)
{
char *s;
const char *spanp;
int c, sc;
size_t l;
char *tok;

if (stringp == NULL || delim == NULL)
return NULL;

if ((s = *stringp) == NULL)
return NULL;
l = strlen(s) + 1;
for (tok = s;;) {
c = *s++;
l--;
while (esc != '\0' && c == esc) {
memmove(s - 1, s, l);
c = *s++;
l--;
}
spanp = delim;
do {
if ((sc = *spanp++) == c) {
if (c == '\0')
s = NULL;
else
s[-1] = '\0';
*stringp = s;
return tok;
}
} while (sc != '\0');
}
}
#endif

