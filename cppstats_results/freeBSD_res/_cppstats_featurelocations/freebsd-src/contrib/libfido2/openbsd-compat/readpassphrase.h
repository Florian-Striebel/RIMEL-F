























#if !defined(_READPASSPHRASE_H_)
#define _READPASSPHRASE_H_

#if !defined(HAVE_READPASSPHRASE)

#include <stdlib.h>

#define RPP_ECHO_OFF 0x00
#define RPP_ECHO_ON 0x01
#define RPP_REQUIRE_TTY 0x02
#define RPP_FORCELOWER 0x04
#define RPP_FORCEUPPER 0x08
#define RPP_SEVENBIT 0x10
#define RPP_STDIN 0x20

char * readpassphrase(const char *, char *, size_t, int);

#endif

#endif
