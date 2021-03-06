






























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if !HAVE_FGETLN
#include <stdlib.h>
#if !defined(HAVE_NBTOOL_CONFIG_H)

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#endif

char *
fgetln(FILE *fp, size_t *len)
{
static char *buf = NULL;
static size_t bufsiz = 0;
char *ptr;


if (buf == NULL) {
bufsiz = BUFSIZ;
if ((buf = malloc(bufsiz)) == NULL)
return NULL;
}

if (fgets(buf, bufsiz, fp) == NULL)
return NULL;

*len = 0;
while ((ptr = strchr(&buf[*len], '\n')) == NULL) {
size_t nbufsiz = bufsiz + BUFSIZ;
char *nbuf = realloc(buf, nbufsiz);

if (nbuf == NULL) {
int oerrno = errno;
free(buf);
errno = oerrno;
buf = NULL;
return NULL;
} else
buf = nbuf;

if (fgets(&buf[bufsiz], BUFSIZ, fp) == NULL) {
buf[bufsiz] = '\0';
*len = strlen(buf);
return buf;
}

*len = bufsiz;
bufsiz = nbufsiz;
}

*len = (ptr - buf) + 1;
return buf;
}

#endif

#if defined(TEST)
int
main(int argc, char *argv[])
{
char *p;
size_t len;

while ((p = fgetln(stdin, &len)) != NULL) {
(void)printf("%zu %s", len, p);
free(p);
}
return 0;
}
#endif
