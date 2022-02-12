


















#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#elif HAVE_STDINT_H
#include <stdint.h>
#endif

#include <stdlib.h>


#define verify(name, assertion) struct name { char a[(assertion) ? 1 : -1]; }

#if defined(UNSIGNED)
#if !defined(HAVE_DECL_STRTOULL)
"this configure-time declaration test was not run"
#endif
#if !HAVE_DECL_STRTOULL && HAVE_UNSIGNED_LONG_LONG
unsigned long long strtoull (char const *, char **, int);
#endif

#else

#if !defined(HAVE_DECL_STRTOLL)
"this configure-time declaration test was not run"
#endif
#if !HAVE_DECL_STRTOLL && HAVE_UNSIGNED_LONG_LONG
long long strtoll (char const *, char **, int);
#endif
#endif

#if defined(UNSIGNED)
#undef HAVE_LONG_LONG
#define HAVE_LONG_LONG HAVE_UNSIGNED_LONG_LONG
#define INT uintmax_t
#define strtoimax strtoumax
#define strtol strtoul
#define strtoll strtoull
#else
#define INT intmax_t
#endif

INT
strtoimax (char const *ptr, char **endptr, int base)
{
#if HAVE_LONG_LONG
verify (size_is_that_of_long_or_long_long,
(sizeof (INT) == sizeof (long)
|| sizeof (INT) == sizeof (long long)));

if (sizeof (INT) != sizeof (long))
return strtoll (ptr, endptr, base);
#else
verify (size_is_that_of_long,
sizeof (INT) == sizeof (long));
#endif

return strtol (ptr, endptr, base);
}
