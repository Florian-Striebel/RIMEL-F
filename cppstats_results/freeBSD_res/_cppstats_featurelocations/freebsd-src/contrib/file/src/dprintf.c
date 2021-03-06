


























#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: dprintf.c,v 1.2 2018/09/09 20:33:28 christos Exp $")
#endif

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

int
dprintf(int fd, const char *fmt, ...)
{
va_list ap;

char buf[1024];
int len;

va_start(ap, fmt);
len = vsnprintf(buf, sizeof(buf), fmt, ap);
va_end(ap);

if ((size_t)len >= sizeof(buf))
return -1;

if (write(fd, buf, (size_t)len) != len)
return -1;

return len;
}
