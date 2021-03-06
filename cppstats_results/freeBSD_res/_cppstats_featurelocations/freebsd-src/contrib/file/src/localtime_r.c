

#include "file.h"
#if !defined(lint)
FILE_RCSID("@(#)$File: localtime_r.c,v 1.2 2015/07/11 14:41:37 christos Exp $")
#endif
#include <time.h>
#include <string.h>


struct tm *
localtime_r(const time_t *t, struct tm *tm)
{
struct tm *tmp = localtime(t);
if (tmp == NULL)
return NULL;
memcpy(tm, tmp, sizeof(*tm));
return tmp;
}
