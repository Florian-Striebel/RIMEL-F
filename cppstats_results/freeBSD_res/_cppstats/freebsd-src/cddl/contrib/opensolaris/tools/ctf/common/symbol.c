#pragma ident "%Z%%M% %I% %E% SMI"
#include <sys/types.h>
#include <string.h>
#include "symbol.h"
int
ignore_symbol(GElf_Sym *sym, const char *name)
{
uchar_t type = GELF_ST_TYPE(sym->st_info);
if (sym->st_shndx == SHN_UNDEF || sym->st_name == 0)
return (1);
if (strcmp(name, "_START_") == 0 || strcmp(name, "_END_") == 0)
return (1);
if (type == STT_OBJECT && sym->st_shndx == SHN_ABS &&
sym->st_value == 0)
return (1);
return (0);
}
