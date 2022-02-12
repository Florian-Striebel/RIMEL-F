

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_finish.c 2074 2011-10-27 03:34:33Z jkoshy $");

Dwarf_Unsigned
dwarf_producer_finish(Dwarf_P_Debug dbg, Dwarf_Error *error)
{

if (dbg == NULL || dbg->dbg_mode != DW_DLC_WRITE) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_NOCOUNT);
}

_dwarf_deinit(dbg);

free(dbg);

return (1);
}
