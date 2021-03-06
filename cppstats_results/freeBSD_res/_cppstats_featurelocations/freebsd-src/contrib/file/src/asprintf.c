



























#include "file.h"

#if !defined(lint)
FILE_RCSID("@(#)$File: asprintf.c,v 1.5 2018/09/09 20:33:28 christos Exp $")
#endif

int asprintf(char **ptr, const char *fmt, ...)
{
va_list vargs;
int retval;

va_start(vargs, fmt);
retval = vasprintf(ptr, fmt, vargs);
va_end(vargs);

return retval;
}
