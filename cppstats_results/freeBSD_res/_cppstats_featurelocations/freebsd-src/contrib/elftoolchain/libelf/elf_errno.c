

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_errno.c 3174 2015-03-27 17:13:41Z emaste $");

int
elf_errno(void)
{
int old;

old = LIBELF_PRIVATE(error);
LIBELF_PRIVATE(error) = 0;
return (old & LIBELF_ELF_ERROR_MASK);
}
