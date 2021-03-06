

























#if !defined(_UTIL_H)
#define _UTIL_H

#pragma ident "%Z%%M% %I% %E% SMI"

#include <libelf.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern int findelfsecidx(Elf *, char *);

extern void die(char *, ...);
extern void elfdie(char *, ...);

#if defined(illumos)
extern const char *progname;
#endif

#if defined(__cplusplus)
}
#endif

#endif
