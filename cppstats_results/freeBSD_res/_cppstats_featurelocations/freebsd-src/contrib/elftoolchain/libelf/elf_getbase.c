

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_getbase.c 3174 2015-03-27 17:13:41Z emaste $");

off_t
elf_getbase(Elf *e)
{
if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return ((off_t) -1);
}

if (e->e_parent == NULL)
return ((off_t) 0);

return ((off_t) ((uintptr_t) e->e_rawfile -
(uintptr_t) e->e_parent->e_rawfile));
}
