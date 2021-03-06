


















#if !defined(XSTRTOL_H_)
#define XSTRTOL_H_ 1

#include "exitfail.h"


#if HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#endif

#if !defined(_STRTOL_ERROR)
enum strtol_error
{
LONGINT_OK = 0,



LONGINT_OVERFLOW = 1,
LONGINT_INVALID_SUFFIX_CHAR = 2,

LONGINT_INVALID_SUFFIX_CHAR_WITH_OVERFLOW = (LONGINT_INVALID_SUFFIX_CHAR
| LONGINT_OVERFLOW),
LONGINT_INVALID = 4
};
typedef enum strtol_error strtol_error;
#endif

#define _DECLARE_XSTRTOL(name, type) strtol_error name (const char *, char **, int, type *, const char *);

_DECLARE_XSTRTOL (xstrtol, long int)
_DECLARE_XSTRTOL (xstrtoul, unsigned long int)
_DECLARE_XSTRTOL (xstrtoimax, intmax_t)
_DECLARE_XSTRTOL (xstrtoumax, uintmax_t)

#define _STRTOL_ERROR(Exit_code, Str, Argument_type_string, Err) do { switch ((Err)) { default: abort (); case LONGINT_INVALID: error ((Exit_code), 0, "invalid %s `%s'", (Argument_type_string), (Str)); break; case LONGINT_INVALID_SUFFIX_CHAR: case LONGINT_INVALID_SUFFIX_CHAR | LONGINT_OVERFLOW: error ((Exit_code), 0, "invalid character following %s in `%s'", (Argument_type_string), (Str)); break; case LONGINT_OVERFLOW: error ((Exit_code), 0, "%s `%s' too large", (Argument_type_string), (Str)); break; } } while (0)


























#define STRTOL_FATAL_ERROR(Str, Argument_type_string, Err) _STRTOL_ERROR (exit_failure, Str, Argument_type_string, Err)


#define STRTOL_FAIL_WARN(Str, Argument_type_string, Err) _STRTOL_ERROR (0, Str, Argument_type_string, Err)


#endif
