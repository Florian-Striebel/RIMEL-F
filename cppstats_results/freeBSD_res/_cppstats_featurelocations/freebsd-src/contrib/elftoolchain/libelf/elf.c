

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf.c 2225 2011-11-26 18:55:54Z jkoshy $");

struct _libelf_globals _libelf = {
.libelf_arch = LIBELF_ARCH,
.libelf_byteorder = LIBELF_BYTEORDER,
.libelf_class = LIBELF_CLASS,
.libelf_error = 0,
.libelf_fillchar = 0,
.libelf_version = EV_NONE
};
