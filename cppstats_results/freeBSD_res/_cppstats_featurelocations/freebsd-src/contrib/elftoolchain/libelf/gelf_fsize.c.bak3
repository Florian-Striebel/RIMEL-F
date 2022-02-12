

























#include <gelf.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: gelf_fsize.c 3174 2015-03-27 17:13:41Z emaste $");

size_t
elf32_fsize(Elf_Type t, size_t c, unsigned int v)
{
return (_libelf_fsize(t, ELFCLASS32, v, c));
}

size_t
elf64_fsize(Elf_Type t, size_t c, unsigned int v)
{
return (_libelf_fsize(t, ELFCLASS64, v, c));
}

size_t
gelf_fsize(Elf *e, Elf_Type t, size_t c, unsigned int v)
{

if (e == NULL) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}

if (e->e_class == ELFCLASS32 || e->e_class == ELFCLASS64)
return (_libelf_fsize(t, e->e_class, v, c));

LIBELF_SET_ERROR(ARGUMENT, 0);
return (0);
}
