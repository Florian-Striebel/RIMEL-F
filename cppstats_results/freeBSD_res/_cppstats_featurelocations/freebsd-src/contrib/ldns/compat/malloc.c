


#if HAVE_CONFIG_H
#include <ldns/config.h>
#endif
#undef malloc

#include <sys/types.h>

void *malloc (size_t n);




void *
rpl_malloc (size_t n)
{
if (n == 0)
n = 1;
return malloc (n);
}
