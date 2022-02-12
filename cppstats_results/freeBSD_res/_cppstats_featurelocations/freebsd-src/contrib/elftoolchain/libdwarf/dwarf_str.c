

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_str.c 3295 2016-01-08 22:08:10Z jkoshy $");

int
dwarf_get_str(Dwarf_Debug dbg, Dwarf_Off offset, char **string,
Dwarf_Signed *ret_strlen, Dwarf_Error *error)
{
Dwarf_Section *ds;

if (dbg == NULL || string == NULL || ret_strlen == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

ds = _dwarf_find_section(dbg, ".debug_str");
if (ds == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

if (offset > ds->ds_size) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (offset == ds->ds_size) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

*string = (char *) ds->ds_data + offset;
*ret_strlen = strlen(*string);

return (DW_DLV_OK);
}
