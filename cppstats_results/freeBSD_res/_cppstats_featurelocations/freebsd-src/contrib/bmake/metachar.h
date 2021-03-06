





























#if !defined(MAKE_METACHAR_H)
#define MAKE_METACHAR_H

#include "make.h"

extern const unsigned char _metachar[];

MAKE_INLINE bool
is_shell_metachar(char c)
{
return _metachar[c & 0x7f] != 0;
}

MAKE_INLINE bool
needshell(const char *cmd)
{
while (!is_shell_metachar(*cmd) && *cmd != ':' && *cmd != '=')
cmd++;
return *cmd != '\0';
}

#endif
