


















#if !defined(XALLOC_H_)
#define XALLOC_H_

#include <stddef.h>

#if !defined(__attribute__)
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8) || __STRICT_ANSI__
#define __attribute__(x)
#endif
#endif

#if !defined(ATTRIBUTE_NORETURN)
#define ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
#endif



extern void (*xalloc_fail_func) (void);




extern char const xalloc_msg_memory_exhausted[];






extern void xalloc_die (void) ATTRIBUTE_NORETURN;

void *xmalloc (size_t s);
void *xnmalloc (size_t n, size_t s);
void *xzalloc (size_t s);
void *xcalloc (size_t n, size_t s);
void *xrealloc (void *p, size_t s);
void *xnrealloc (void *p, size_t n, size_t s);
void *x2realloc (void *p, size_t *pn);
void *x2nrealloc (void *p, size_t *pn, size_t s);
void *xclone (void const *p, size_t s);
char *xstrdup (const char *str);













#define xalloc_oversized(n, s) ((size_t) (sizeof (ptrdiff_t) <= sizeof (size_t) ? -1 : -2) / (s) < (n))




#define CCLONE(p, n) xclone (p, (n) * sizeof *(p))
#define CLONE(p) xclone (p, sizeof *(p))
#define NEW(type, var) type *var = xmalloc (sizeof (type))
#define XCALLOC(type, n) xcalloc (n, sizeof (type))
#define XMALLOC(type, n) xnmalloc (n, sizeof (type))
#define XREALLOC(p, type, n) xnrealloc (p, n, sizeof (type))
#define XFREE(p) free (p)

#endif
