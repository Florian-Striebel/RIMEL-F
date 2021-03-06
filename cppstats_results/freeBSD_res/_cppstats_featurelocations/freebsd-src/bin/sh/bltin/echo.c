



































#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");





#define main echocmd

#include "bltin.h"



int
main(int argc, char *argv[])
{
char **ap;
char *p;
char c;
int count;
int nflag = 0;
#if !defined(eflag)
int eflag = 0;
#endif

ap = argv;
if (argc)
ap++;
if ((p = *ap) != NULL) {
if (equal(p, "-n")) {
nflag++;
ap++;
} else if (equal(p, "-e")) {
#if !defined(eflag)
eflag++;
#endif
ap++;
}
}
while ((p = *ap++) != NULL) {
while ((c = *p++) != '\0') {
if (c == '\\' && eflag) {
switch (*p++) {
case 'a': c = '\a'; break;
case 'b': c = '\b'; break;
case 'c': return 0;
case 'e': c = '\033'; break;
case 'f': c = '\f'; break;
case 'n': c = '\n'; break;
case 'r': c = '\r'; break;
case 't': c = '\t'; break;
case 'v': c = '\v'; break;
case '\\': break;
case '0':
c = 0;
count = 3;
while (--count >= 0 && (unsigned)(*p - '0') < 8)
c = (c << 3) + (*p++ - '0');
break;
default:
p--;
break;
}
}
putchar(c);
}
if (*ap)
putchar(' ');
}
if (! nflag)
putchar('\n');
return 0;
}
