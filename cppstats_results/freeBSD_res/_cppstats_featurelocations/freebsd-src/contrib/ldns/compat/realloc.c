


#if HAVE_CONFIG_H
#include <ldns/config.h>
#endif
#undef realloc

#include <sys/types.h>

void *realloc (void*, size_t);
void *malloc (size_t);







void *
rpl_realloc (void* ptr, size_t n)
{
if (n == 0)
n = 1;
if(ptr == 0) {
return malloc(n);
}
return realloc(ptr, n);
}

