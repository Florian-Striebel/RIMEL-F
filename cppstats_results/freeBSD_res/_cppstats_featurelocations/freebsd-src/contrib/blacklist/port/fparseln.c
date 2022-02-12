
























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: fparseln.c,v 1.1 2015/01/22 03:48:07 christos Exp $");
#endif

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if ! HAVE_FPARSELN || BROKEN_FPARSELN

#define FLOCKFILE(fp)
#define FUNLOCKFILE(fp)

#if defined(_REENTRANT) && !HAVE_NBTOOL_CONFIG_H
#define __fgetln(f, l) __fgetstr(f, l, '\n')
#else
#define __fgetln(f, l) fgetln(f, l)
#endif

static int isescaped(const char *, const char *, int);





static int
isescaped(const char *sp, const char *p, int esc)
{
const char *cp;
size_t ne;


if (esc == '\0')
return 0;


for (ne = 0, cp = p; --cp >= sp && *cp == esc; ne++)
continue;


return (ne & 1) != 0;
}







char *
fparseln(FILE *fp, size_t *size, size_t *lineno, const char str[3], int flags)
{
static const char dstr[3] = { '\\', '\\', '#' };

size_t s, len;
char *buf;
char *ptr, *cp;
int cnt;
char esc, con, nl, com;

len = 0;
buf = NULL;
cnt = 1;

if (str == NULL)
str = dstr;

esc = str[0];
con = str[1];
com = str[2];




nl = '\n';

FLOCKFILE(fp);

while (cnt) {
cnt = 0;

if (lineno)
(*lineno)++;

if ((ptr = __fgetln(fp, &s)) == NULL)
break;

if (s && com) {
for (cp = ptr; cp < ptr + s; cp++)
if (*cp == com && !isescaped(ptr, cp, esc)) {
s = cp - ptr;
cnt = s == 0 && buf == NULL;
break;
}
}

if (s && nl) {
cp = &ptr[s - 1];

if (*cp == nl)
s--;
}

if (s && con) {
cp = &ptr[s - 1];

if (*cp == con && !isescaped(ptr, cp, esc)) {
s--;
cnt = 1;
}
}

if (s == 0) {




if (cnt || buf != NULL)
continue;
}

if ((cp = realloc(buf, len + s + 1)) == NULL) {
FUNLOCKFILE(fp);
free(buf);
return NULL;
}
buf = cp;

(void) memcpy(buf + len, ptr, s);
len += s;
buf[len] = '\0';
}

FUNLOCKFILE(fp);

if ((flags & FPARSELN_UNESCALL) != 0 && esc && buf != NULL &&
strchr(buf, esc) != NULL) {
ptr = cp = buf;
while (cp[0] != '\0') {
int skipesc;

while (cp[0] != '\0' && cp[0] != esc)
*ptr++ = *cp++;
if (cp[0] == '\0' || cp[1] == '\0')
break;

skipesc = 0;
if (cp[1] == com)
skipesc += (flags & FPARSELN_UNESCCOMM);
if (cp[1] == con)
skipesc += (flags & FPARSELN_UNESCCONT);
if (cp[1] == esc)
skipesc += (flags & FPARSELN_UNESCESC);
if (cp[1] != com && cp[1] != con && cp[1] != esc)
skipesc = (flags & FPARSELN_UNESCREST);

if (skipesc)
cp++;
else
*ptr++ = *cp++;
*ptr++ = *cp++;
}
*ptr = '\0';
len = strlen(buf);
}

if (size)
*size = len;
return buf;
}

#if defined(TEST)

int main(int, char **);

int
main(int argc, char **argv)
{
char *ptr;
size_t size, line;

line = 0;
while ((ptr = fparseln(stdin, &size, &line, NULL,
FPARSELN_UNESCALL)) != NULL)
printf("line %d (%d) |%s|\n", line, size, ptr);
return 0;
}















#endif
#endif
