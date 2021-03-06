


















#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

#include "gettext.h"
#define _(msgid) gettext (msgid)
#define N_(msgid) msgid

#include "error.h"
#include "exitfail.h"

#if !defined(SIZE_MAX)
#define SIZE_MAX ((size_t) -1)
#endif

#if !defined(HAVE_MALLOC)
"you must run the autoconf test for a GNU libc compatible malloc"
#endif

#if !defined(HAVE_REALLOC)
"you must run the autoconf test for a GNU libc compatible realloc"
#endif


void (*xalloc_fail_func) (void) = 0;



char const xalloc_msg_memory_exhausted[] = N_("memory exhausted");

void
xalloc_die (void)
{
if (xalloc_fail_func)
(*xalloc_fail_func) ();
error (exit_failure, 0, "%s", _(xalloc_msg_memory_exhausted));



abort ();
}




static inline void *
xnmalloc_inline (size_t n, size_t s)
{
void *p;
if (xalloc_oversized (n, s) || ! (p = malloc (n * s)))
xalloc_die ();
return p;
}

void *
xnmalloc (size_t n, size_t s)
{
return xnmalloc_inline (n, s);
}



void *
xmalloc (size_t n)
{
return xnmalloc_inline (n, 1);
}




static inline void *
xnrealloc_inline (void *p, size_t n, size_t s)
{
if (xalloc_oversized (n, s) || ! (p = realloc (p, n * s)))
xalloc_die ();
return p;
}

void *
xnrealloc (void *p, size_t n, size_t s)
{
return xnrealloc_inline (p, n, s);
}




void *
xrealloc (void *p, size_t n)
{
return xnrealloc_inline (p, n, 1);
}

























































static inline void *
x2nrealloc_inline (void *p, size_t *pn, size_t s)
{
size_t n = *pn;

if (! p)
{
if (! n)
{




enum { DEFAULT_MXFAST = 64 };

n = DEFAULT_MXFAST / s;
n += !n;
}
}
else
{
if (SIZE_MAX / 2 / s < n)
xalloc_die ();
n *= 2;
}

*pn = n;
return xrealloc (p, n * s);
}

void *
x2nrealloc (void *p, size_t *pn, size_t s)
{
return x2nrealloc_inline (p, pn, s);
}







void *
x2realloc (void *p, size_t *pn)
{
return x2nrealloc_inline (p, pn, 1);
}





void *
xzalloc (size_t s)
{
return memset (xmalloc (s), 0, s);
}




void *
xcalloc (size_t n, size_t s)
{
void *p;


if (xalloc_oversized (n, s) || ! (p = calloc (n, s)))
xalloc_die ();
return p;
}





void *
xclone (void const *p, size_t s)
{
return memcpy (xmalloc (s), p, s);
}
