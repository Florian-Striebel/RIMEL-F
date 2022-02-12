

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

#include "utils.h"



static const char *pname;

#pragma init(getpname)
const char *
getpname(void)
{
const char *p, *q;

if (pname != NULL)
return (pname);

if ((p = getexecname()) != NULL)
q = strrchr(p, '/');
else
q = NULL;

if (q == NULL)
pname = p;
else
pname = q + 1;

return (pname);
}

void
vwarn(const char *format, va_list alist)
{
int err = errno;

if (pname != NULL)
(void) fprintf(stderr, "%s: ", pname);

(void) vfprintf(stderr, format, alist);

if (strchr(format, '\n') == NULL)
(void) fprintf(stderr, ": %s\n", strerror(err));
}


void
warn(const char *format, ...)
{
va_list alist;

va_start(alist, format);
vwarn(format, alist);
va_end(alist);
}

void
vdie(const char *format, va_list alist)
{
vwarn(format, alist);
exit(E_ERROR);
}


void
die(const char *format, ...)
{
va_list alist;

va_start(alist, format);
vdie(format, alist);
va_end(alist);
}
