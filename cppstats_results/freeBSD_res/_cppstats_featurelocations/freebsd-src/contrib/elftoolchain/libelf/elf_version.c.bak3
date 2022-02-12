

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_version.c 3174 2015-03-27 17:13:41Z emaste $");

unsigned int
elf_version(unsigned int v)
{
unsigned int old;

if ((old = LIBELF_PRIVATE(version)) == EV_NONE)
old = EV_CURRENT;

if (v == EV_NONE)
return old;
if (v > EV_CURRENT) {
LIBELF_SET_ERROR(VERSION, 0);
return EV_NONE;
}

LIBELF_PRIVATE(version) = v;
return (old);
}
