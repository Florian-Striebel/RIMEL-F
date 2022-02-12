

























#if !defined(_MEMORY_H)
#define _MEMORY_H

#pragma ident "%Z%%M% %I% %E% SMI"





#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

void *xmalloc(size_t);
void *xcalloc(size_t);
char *xstrdup(const char *);
char *xstrndup(char *, size_t);
void *xrealloc(void *, size_t);

#if defined(__cplusplus)
}
#endif

#endif
