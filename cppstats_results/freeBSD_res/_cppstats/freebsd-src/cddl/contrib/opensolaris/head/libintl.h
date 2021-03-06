#include <sys/isa_defs.h>
#if defined(__cplusplus)
extern "C" {
#endif
#if !defined(__cplusplus) || (__cplusplus < 199711L && !defined(__GNUG__))
#if !defined(_WCHAR_T)
#define _WCHAR_T
#if defined(_LP64)
typedef int wchar_t;
#else
typedef long wchar_t;
#endif
#endif
#endif
#define TEXTDOMAINMAX 256
#define __GNU_GETTEXT_SUPPORTED_REVISION(m) ((((m) == 0) || ((m) == 1)) ? 1 : -1)
extern char *dcgettext(const char *, const char *, const int);
extern char *dgettext(const char *, const char *);
extern char *gettext(const char *);
extern char *textdomain(const char *);
extern char *bindtextdomain(const char *, const char *);
extern char *dcngettext(const char *, const char *,
const char *, unsigned long int, int);
extern char *dngettext(const char *, const char *,
const char *, unsigned long int);
extern char *ngettext(const char *, const char *, unsigned long int);
extern char *bind_textdomain_codeset(const char *, const char *);
extern int wdinit(void);
extern int wdchkind(wchar_t);
extern int wdbindf(wchar_t, wchar_t, int);
extern wchar_t *wddelim(wchar_t, wchar_t, int);
extern wchar_t mcfiller(void);
extern int mcwrap(void);
#if defined(__cplusplus)
}
#endif
