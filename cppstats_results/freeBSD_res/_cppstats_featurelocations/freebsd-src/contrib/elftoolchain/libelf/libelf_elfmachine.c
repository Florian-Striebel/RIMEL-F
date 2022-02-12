

























#include <assert.h>
#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id$");





int
_libelf_elfmachine(Elf *e)
{
Elf32_Ehdr *eh32;
Elf64_Ehdr *eh64;

if (!e)
return EM_NONE;

assert(e->e_kind == ELF_K_ELF);
assert(e->e_class != ELFCLASSNONE);

eh32 = NULL;
eh64 = NULL;

switch (e->e_class) {
case ELFCLASS32:
eh32 = e->e_u.e_elf.e_ehdr.e_ehdr32;
return eh32 ? eh32->e_machine : EM_NONE;
case ELFCLASS64:
eh64 = e->e_u.e_elf.e_ehdr.e_ehdr64;
return eh64 ? eh64->e_machine : EM_NONE;
}

return (EM_NONE);
}
