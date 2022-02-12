

























#include <libelf.h>

#include "_libelf.h"

ELFTC_VCSID("$Id: libelf_data.c 3737 2019-05-05 14:49:50Z jkoshy $");

int
_libelf_xlate_shtype(uint32_t sht)
{



switch (sht) {
case SHT_DYNAMIC:
return (ELF_T_DYN);
case SHT_DYNSYM:
return (ELF_T_SYM);
case SHT_FINI_ARRAY:
return (ELF_T_ADDR);
case SHT_GNU_HASH:
return (ELF_T_GNUHASH);
case SHT_GNU_LIBLIST:
return (ELF_T_WORD);
case SHT_GROUP:
return (ELF_T_WORD);
case SHT_HASH:
return (ELF_T_WORD);
case SHT_INIT_ARRAY:
return (ELF_T_ADDR);
case SHT_NOBITS:
return (ELF_T_BYTE);
case SHT_NOTE:
return (ELF_T_NOTE);
case SHT_PREINIT_ARRAY:
return (ELF_T_ADDR);
case SHT_PROGBITS:
return (ELF_T_BYTE);
case SHT_REL:
return (ELF_T_REL);
case SHT_RELA:
return (ELF_T_RELA);
case SHT_STRTAB:
return (ELF_T_BYTE);
case SHT_SYMTAB:
return (ELF_T_SYM);
case SHT_SYMTAB_SHNDX:
return (ELF_T_WORD);
case SHT_SUNW_dof:
return (ELF_T_BYTE);
case SHT_SUNW_move:
return (ELF_T_MOVE);
case SHT_SUNW_syminfo:
return (ELF_T_SYMINFO);
case SHT_SUNW_verdef:
return (ELF_T_VDEF);
case SHT_SUNW_verneed:
return (ELF_T_VNEED);
case SHT_SUNW_versym:
return (ELF_T_HALF);
default:












if (sht >= SHT_LOOS)
return (ELF_T_BYTE);




return (-1);
}
}
