























#if !defined(_DT_STRING_H)
#define _DT_STRING_H


#include <sys/types.h>
#include <strings.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern size_t stresc2chr(char *);
extern char *strchr2esc(const char *, size_t);
extern const char *strbasename(const char *);
extern const char *strbadidnum(const char *);
extern int strisglob(const char *);
extern char *strhyphenate(char *);

#if defined(__cplusplus)
}
#endif

#endif
