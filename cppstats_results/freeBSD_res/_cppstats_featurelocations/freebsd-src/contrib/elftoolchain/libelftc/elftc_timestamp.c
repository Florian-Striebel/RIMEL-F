




























#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <libelftc.h>

int
elftc_timestamp(time_t *timestamp)
{
long long source_date_epoch;
char *env, *eptr;

if ((env = getenv("SOURCE_DATE_EPOCH")) != NULL) {
errno = 0;
source_date_epoch = strtoll(env, &eptr, 10);
if (*eptr != '\0')
errno = EINVAL;
if (source_date_epoch < 0)
errno = ERANGE;
if (errno != 0)
return (-1);
*timestamp = source_date_epoch;
return (0);
}
*timestamp = time(NULL);
return (0);
}
