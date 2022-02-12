

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_arange.c 2074 2011-10-27 03:34:33Z jkoshy $");

Dwarf_Unsigned
dwarf_add_arange(Dwarf_P_Debug dbg, Dwarf_Addr start, Dwarf_Unsigned length,
Dwarf_Signed symbol_index, Dwarf_Error *error)
{

return (dwarf_add_arange_b(dbg, start, length, symbol_index, 0, 0,
error));
}

Dwarf_Unsigned
dwarf_add_arange_b(Dwarf_P_Debug dbg, Dwarf_Addr start, Dwarf_Unsigned length,
Dwarf_Unsigned symbol_index, Dwarf_Unsigned end_symbol_index,
Dwarf_Addr offset_from_end_symbol, Dwarf_Error *error)
{
Dwarf_ArangeSet as;
Dwarf_Arange ar;

if (dbg == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (0);
}
as = dbg->dbgp_as;

if (end_symbol_index > 0 &&
(dbg->dbgp_flags & DW_DLC_SYMBOLIC_RELOCATIONS) == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (0);
}

if ((ar = calloc(1, sizeof(struct _Dwarf_Arange))) == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (0);
}
ar->ar_as = as;
ar->ar_address = start;
ar->ar_range = length;
ar->ar_symndx = symbol_index;
ar->ar_esymndx = end_symbol_index;
ar->ar_eoff = offset_from_end_symbol;
STAILQ_INSERT_TAIL(&as->as_arlist, ar, ar_next);

return (1);
}
