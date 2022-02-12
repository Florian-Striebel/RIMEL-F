

























#include <ar.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_shnum.c 3174 2015-03-27 17:13:41Z emaste $");

static int
_libelf_getshdrnum(Elf *e, size_t *shnum)
{
void *eh;
int ec;

if (e == NULL || e->e_kind != ELF_K_ELF ||
((ec = e->e_class) != ELFCLASS32 && ec != ELFCLASS64)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (-1);
}

if ((eh = _libelf_ehdr(e, ec, 0)) == NULL)
return (-1);

*shnum = e->e_u.e_elf.e_nscn;

return (0);
}

int
elf_getshdrnum(Elf *e, size_t *shnum)
{
return (_libelf_getshdrnum(e, shnum));
}


int
elf_getshnum(Elf *e, size_t *shnum)
{
return (_libelf_getshdrnum(e, shnum) >= 0);
}
