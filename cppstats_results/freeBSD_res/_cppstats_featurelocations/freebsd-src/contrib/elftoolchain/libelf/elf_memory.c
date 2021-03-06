

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_memory.c 3013 2014-03-23 06:16:59Z jkoshy $");

Elf *
elf_memory(char *image, size_t sz)
{
if (LIBELF_PRIVATE(version) == EV_NONE) {
LIBELF_SET_ERROR(SEQUENCE, 0);
return (NULL);
}

if (image == NULL || sz == 0) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

return (_libelf_memory((unsigned char *) image, sz, 1));
}
