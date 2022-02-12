

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_cntl.c 2225 2011-11-26 18:55:54Z jkoshy $");

int
elf_cntl(Elf *e, Elf_Cmd c)
{
if (e == NULL ||
(c != ELF_C_FDDONE && c != ELF_C_FDREAD)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (-1);
}

if (e->e_parent) {
LIBELF_SET_ERROR(ARCHIVE, 0);
return (-1);
}

if (c == ELF_C_FDREAD) {
if (e->e_cmd == ELF_C_WRITE) {
LIBELF_SET_ERROR(MODE, 0);
return (-1);
}
else
return (0);
}

e->e_fd = -1;
return 0;
}
