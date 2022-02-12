




#if defined(_MSC_VER)
#if _MSC_VER >= 1900
#include <../ucrt/sys/types.h>
#else
#include <../include/sys/types.h>
#endif
#endif

#if !defined(_COMPAT_TYPES_H)
#define _COMPAT_TYPES_H

#include <stdint.h>

#if defined(__MINGW32__)
#include <_bsd_types.h>
typedef uint32_t in_addr_t;
typedef uint32_t uid_t;
#endif

#if defined(_MSC_VER)
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

#include <basetsd.h>
typedef SSIZE_T ssize_t;

#if !defined(SSIZE_MAX)
#if defined(_WIN64)
#define SSIZE_MAX _I64_MAX
#else
#define SSIZE_MAX INT_MAX
#endif
#endif

#endif

#if !defined(HAVE_ATTRIBUTE__BOUNDED__) && !defined(__bounded__)
#define __bounded__(x, y, z)
#endif

#if defined(_WIN32)
#define __warn_references(sym,msg)
#else

#if !defined(__warn_references)

#if !defined(__STRING)
#define __STRING(x) #x
#endif

#if defined(__GNUC__) && defined (HAS_GNU_WARNING_LONG)
#define __warn_references(sym,msg) __asm__(".section .gnu.warning." __STRING(sym) "\n\t.ascii \"" msg "\"\n\t.text");


#else
#define __warn_references(sym,msg)
#endif

#endif
#endif

#endif
