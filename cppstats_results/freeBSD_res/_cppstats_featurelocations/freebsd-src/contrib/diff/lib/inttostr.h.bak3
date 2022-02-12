



















#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include <limits.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif




#define INT_STRLEN_BOUND(t) ((sizeof (t) * CHAR_BIT - 1) * 302 / 1000 + 2)

#define INT_BUFSIZE_BOUND(t) (INT_STRLEN_BOUND (t) + 1)

char *offtostr (off_t, char *);
char *imaxtostr (intmax_t, char *);
char *umaxtostr (uintmax_t, char *);
