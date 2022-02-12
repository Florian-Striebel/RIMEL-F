

























#include <ar.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_shstrndx.c 3174 2015-03-27 17:13:41Z emaste $");

static int
_libelf_getshdrstrndx(Elf *e, size_t *strndx)
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

*strndx = e->e_u.e_elf.e_strndx;

return (0);
}

int
elf_getshdrstrndx(Elf *e, size_t *strndx)
{
return (_libelf_getshdrstrndx(e, strndx));
}

int
elf_getshstrndx(Elf *e, size_t *strndx)
{
return (_libelf_getshdrstrndx(e, strndx) >= 0);
}

int
elf_setshstrndx(Elf *e, size_t strndx)
{
void *eh;
int ec;

if (e == NULL || e->e_kind != ELF_K_ELF ||
((ec = e->e_class) != ELFCLASS32 && ec != ELFCLASS64) ||
((eh = _libelf_ehdr(e, ec, 0)) == NULL)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

return (_libelf_setshstrndx(e, eh, ec, strndx));
}
