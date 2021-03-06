

























#include <gelf.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_checksum.c 3174 2015-03-27 17:13:41Z emaste $");

long
elf32_checksum(Elf *e)
{
return (_libelf_checksum(e, ELFCLASS32));
}

long
elf64_checksum(Elf *e)
{
return (_libelf_checksum(e, ELFCLASS64));
}

long
gelf_checksum(Elf *e)
{
int ec;
if (e == NULL ||
((ec = e->e_class) != ELFCLASS32 && ec != ELFCLASS64)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0L);
}
return (_libelf_checksum(e, ec));
}
