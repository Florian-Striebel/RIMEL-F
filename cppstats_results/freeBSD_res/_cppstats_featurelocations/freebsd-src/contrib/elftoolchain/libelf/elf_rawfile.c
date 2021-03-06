

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_rawfile.c 3712 2019-03-16 22:23:34Z jkoshy $");

char *
elf_rawfile(Elf *e, size_t *sz)
{
unsigned char *ptr;

ptr = NULL;

if (e == NULL)
LIBELF_SET_ERROR(ARGUMENT, 0);
else if ((ptr = e->e_rawfile) == NULL && e->e_cmd == ELF_C_WRITE)
LIBELF_SET_ERROR(SEQUENCE, 0);

if (sz)
*sz = e ? (size_t) e->e_rawsize : 0;

return ((char *) ptr);
}
