

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id$");





Elf *
elf_open(int fd)
{
if (LIBELF_PRIVATE(version) == EV_NONE) {
LIBELF_SET_ERROR(SEQUENCE, 0);
return (NULL);
}

return (_libelf_open_object(fd, ELF_C_READ, 0));
}






Elf *
elf_openmemory(char *image, size_t sz)
{
if (LIBELF_PRIVATE(version) == EV_NONE) {
LIBELF_SET_ERROR(SEQUENCE, 0);
return (NULL);
}

if (image == NULL || sz == 0) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

return (_libelf_memory((unsigned char *) image, sz, 0));
}
