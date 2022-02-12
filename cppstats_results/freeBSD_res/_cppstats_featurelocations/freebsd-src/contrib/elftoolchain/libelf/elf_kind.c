

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: elf_kind.c 3174 2015-03-27 17:13:41Z emaste $");

Elf_Kind
elf_kind(Elf *e)
{
if (e == NULL)
return (ELF_K_NONE);
if (e->e_kind == ELF_K_AR ||
e->e_kind == ELF_K_ELF)
return (e->e_kind);
return (ELF_K_NONE);
}
