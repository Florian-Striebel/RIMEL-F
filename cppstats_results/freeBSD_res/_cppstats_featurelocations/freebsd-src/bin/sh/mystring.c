

































#if !defined(lint)
#if 0
static char sccsid[] = "@(#)mystring.c 8.2 (Berkeley) 5/4/95";
#endif
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");









#include <stdlib.h>
#include "shell.h"
#include "syntax.h"
#include "error.h"
#include "mystring.h"


char nullstr[1];











int
number(const char *s)
{
if (! is_number(s))
error("Illegal number: %s", s);
return atoi(s);
}







int
is_number(const char *p)
{
const char *q;

if (*p == '\0')
return 0;
while (*p == '0')
p++;
for (q = p; *q != '\0'; q++)
if (! is_digit(*q))
return 0;
if (q - p > 10 ||
(q - p == 10 && memcmp(p, "2147483647", 10) > 0))
return 0;
return 1;
}
