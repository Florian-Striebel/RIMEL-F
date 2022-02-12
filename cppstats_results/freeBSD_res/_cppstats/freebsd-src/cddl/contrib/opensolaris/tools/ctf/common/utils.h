#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdarg.h>
#if defined(__cplusplus)
extern "C" {
#endif
#define E_SUCCESS 0
#define E_ERROR 1
#define E_USAGE 2
extern void vwarn(const char *, va_list);
extern void warn(const char *, ...);
extern void vdie(const char *, va_list);
extern void die(const char *, ...);
extern const char *getpname(void);
#if defined(__cplusplus)
}
#endif
