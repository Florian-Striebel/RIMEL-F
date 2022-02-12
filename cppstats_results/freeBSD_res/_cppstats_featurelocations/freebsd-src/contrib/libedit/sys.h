






































#if !defined(_h_sys)
#define _h_sys

#if defined(HAVE_SYS_CDEFS_H)
#include <sys/cdefs.h>
#endif

#if !defined(__attribute__) && (defined(__cplusplus) || !defined(__GNUC__) || __GNUC__ == 2 && __GNUC_MINOR__ < 8)
#define __attribute__(A)
#endif

#if !defined(__BEGIN_DECLS)
#if defined(__cplusplus)
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif


#define libedit_private __attribute__((__visibility__("hidden")))

#if !defined(__arraycount)
#define __arraycount(a) (sizeof(a) / sizeof(*(a)))
#endif

#include <stdio.h>

#if !defined(HAVE_STRLCAT)
#define strlcat libedit_strlcat
size_t strlcat(char *dst, const char *src, size_t size);
#endif

#if !defined(HAVE_STRLCPY)
#define strlcpy libedit_strlcpy
size_t strlcpy(char *dst, const char *src, size_t size);
#endif

#if !defined(HAVE_GETLINE)
#define getline libedit_getline
ssize_t getline(char **line, size_t *len, FILE *fp);
#endif

#if !defined(_DIAGASSERT)
#define _DIAGASSERT(x)
#endif

#if !defined(__RCSID)
#define __RCSID(x)
#endif

#if !defined(HAVE_U_INT32_T)
typedef unsigned int u_int32_t;
#endif

#if !defined(HAVE_SIZE_MAX)
#define SIZE_MAX ((size_t)-1)
#endif

#define REGEX
#undef REGEXP

#if defined(__sun)
extern int tgetent(char *, const char *);
extern int tgetflag(char *);
extern int tgetnum(char *);
extern int tputs(const char *, int, int (*)(int));
extern char* tgoto(const char*, int, int);
extern char* tgetstr(char*, char**);
#endif

#endif
