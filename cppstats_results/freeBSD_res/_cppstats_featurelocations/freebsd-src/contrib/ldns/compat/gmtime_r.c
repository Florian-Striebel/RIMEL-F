#if defined(HAVE_CONFIG_H)
#include <ldns/config.h>
#endif

#if defined(HAVE_TIME_H)
#include <time.h>
#endif

struct tm *gmtime_r(const time_t *timep, struct tm *result)
{

*result = *gmtime(timep);
return result;
}
