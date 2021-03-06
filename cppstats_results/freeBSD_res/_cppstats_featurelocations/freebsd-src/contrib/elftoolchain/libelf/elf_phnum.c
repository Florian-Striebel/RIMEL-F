

























#include <ar.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_phnum.c 3174 2015-03-27 17:13:41Z emaste $");

static int
_libelf_getphdrnum(Elf *e, size_t *phnum)
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

*phnum = e->e_u.e_elf.e_nphdr;

return (0);
}

int
elf_getphdrnum(Elf *e, size_t *phnum)
{
return (_libelf_getphdrnum(e, phnum));
}


int
elf_getphnum(Elf *e, size_t *phnum)
{
return (_libelf_getphdrnum(e, phnum) >= 0);
}
