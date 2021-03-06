
























#include <gelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id$");

int
_libelf_is_mips64el(Elf *e)
{

return (e->e_kind == ELF_K_ELF && e->e_class == ELFCLASS64 &&
e->e_u.e_elf.e_ehdr.e_ehdr64->e_machine == EM_MIPS &&
e->e_u.e_elf.e_ehdr.e_ehdr64->e_ident[EI_DATA] == ELFDATA2LSB);
}







Elf64_Xword
_libelf_mips64el_r_info_tof(Elf64_Xword r_info)
{
Elf64_Xword new_info;
uint8_t ssym, type3, type2, type;

ssym = r_info >> 24;
type3 = r_info >> 16;
type2 = r_info >> 8;
type = r_info;
new_info = r_info >> 32;
new_info |= (Elf64_Xword)ssym << 32;
new_info |= (Elf64_Xword)type3 << 40;
new_info |= (Elf64_Xword)type2 << 48;
new_info |= (Elf64_Xword)type << 56;
return (new_info);
}

Elf64_Xword
_libelf_mips64el_r_info_tom(Elf64_Xword r_info)
{
Elf64_Xword new_info;
uint8_t ssym, type3, type2, type;

ssym = r_info >> 32;
type3 = r_info >> 40;
type2 = r_info >> 48;
type = r_info >> 56;
new_info = (r_info & 0xffffffff) << 32;
new_info |= (Elf64_Xword)ssym << 24;
new_info |= (Elf64_Xword)type3 << 16;
new_info |= (Elf64_Xword)type2 << 8;
new_info |= (Elf64_Xword)type;
return (new_info);
}
