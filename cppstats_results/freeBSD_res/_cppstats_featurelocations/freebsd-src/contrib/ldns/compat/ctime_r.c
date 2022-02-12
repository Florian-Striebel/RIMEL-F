#if defined(HAVE_CONFIG_H)
#include <ldns/config.h>
#endif

#if defined(HAVE_TIME_H)
#include <time.h>
#endif

char *ctime_r(const time_t *timep, char *buf)
{

char* result = ctime(timep);
if(buf && result)
strcpy(buf, result);
return result;
}
