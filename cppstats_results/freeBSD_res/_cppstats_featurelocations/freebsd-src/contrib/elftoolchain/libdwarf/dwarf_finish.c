

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_finish.c 2073 2011-10-27 03:30:47Z jkoshy $");

int
dwarf_finish(Dwarf_Debug dbg, Dwarf_Error *error)
{
(void) error;

if (dbg == NULL)
return (DW_DLV_OK);

_dwarf_deinit(dbg);
_dwarf_elf_deinit(dbg);

free(dbg);

return (DW_DLV_OK);
}


int
dwarf_object_finish(Dwarf_Debug dbg, Dwarf_Error *error)
{
(void) error;

if (dbg == NULL)
return (DW_DLV_OK);

_dwarf_deinit(dbg);

free(dbg);

return (DW_DLV_OK);
}
