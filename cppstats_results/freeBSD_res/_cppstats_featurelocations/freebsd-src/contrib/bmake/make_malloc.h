



























#if !defined(USE_EMALLOC)
void *bmake_malloc(size_t);
void *bmake_realloc(void *, size_t);
char *bmake_strdup(const char *);
char *bmake_strldup(const char *, size_t);
#else
#include <util.h>
#define bmake_malloc(n) emalloc(n)
#define bmake_realloc(p, n) erealloc(p, n)
#define bmake_strdup(s) estrdup(s)
#define bmake_strldup(s, n) estrndup(s, n)
#endif

char *bmake_strsedup(const char *, const char *);








MAKE_INLINE void
bmake_free(void *p)
{
if (p != NULL)
free(p);
}
