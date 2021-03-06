

























#include <gelf.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_shdr.c 3174 2015-03-27 17:13:41Z emaste $");

void *
_libelf_getshdr(Elf_Scn *s, int ec)
{
Elf *e;

if (s == NULL || (e = s->s_elf) == NULL ||
e->e_kind != ELF_K_ELF) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

if (ec == ELFCLASSNONE)
ec = e->e_class;

if (ec != e->e_class) {
LIBELF_SET_ERROR(CLASS, 0);
return (NULL);
}

return ((void *) &s->s_shdr);
}
