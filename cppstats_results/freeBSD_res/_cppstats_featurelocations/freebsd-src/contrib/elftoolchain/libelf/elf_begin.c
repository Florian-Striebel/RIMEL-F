

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_begin.c 2364 2011-12-28 17:55:25Z jkoshy $");

Elf *
elf_begin(int fd, Elf_Cmd c, Elf *a)
{
Elf *e;

e = NULL;

if (LIBELF_PRIVATE(version) == EV_NONE) {
LIBELF_SET_ERROR(SEQUENCE, 0);
return (NULL);
}

switch (c) {
case ELF_C_NULL:
return (NULL);

case ELF_C_WRITE:




a = NULL;
break;

case ELF_C_RDWR:
if (a != NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

case ELF_C_READ:






if (a &&
((a->e_fd != -1 && a->e_fd != fd) || c != a->e_cmd)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}
break;

default:
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);

}

if (a == NULL)
e = _libelf_open_object(fd, c, 1);
else if (a->e_kind == ELF_K_AR)
e = _libelf_ar_open_member(a->e_fd, c, a);
else
(e = a)->e_activations++;

return (e);
}
