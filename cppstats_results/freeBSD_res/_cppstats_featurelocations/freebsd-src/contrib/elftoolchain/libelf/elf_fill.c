

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_fill.c 3174 2015-03-27 17:13:41Z emaste $");

void
elf_fill(int fill)
{
LIBELF_PRIVATE(fillchar) = fill;
}
