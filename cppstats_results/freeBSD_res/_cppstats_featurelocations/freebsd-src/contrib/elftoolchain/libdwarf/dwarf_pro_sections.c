

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_sections.c 2074 2011-10-27 03:34:33Z jkoshy $");

Dwarf_Signed
dwarf_transform_to_disk_form(Dwarf_P_Debug dbg, Dwarf_Error *error)
{

if (dbg == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_NOCOUNT);
}

if (_dwarf_generate_sections(dbg, error) != DW_DLE_NONE)
return (DW_DLV_NOCOUNT);

return (dbg->dbgp_seccnt);
}

Dwarf_Ptr
dwarf_get_section_bytes(Dwarf_P_Debug dbg, Dwarf_Signed dwarf_section,
Dwarf_Signed *elf_section_index, Dwarf_Unsigned *length, Dwarf_Error *error)
{
Dwarf_Ptr data;

(void) dwarf_section;

if (dbg == NULL || elf_section_index == NULL || length == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (NULL);
}

if (dbg->dbgp_secpos == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (NULL);
}

*elf_section_index = dbg->dbgp_secpos->ds_ndx;
*length = dbg->dbgp_secpos->ds_size;
data = dbg->dbgp_secpos->ds_data;

dbg->dbgp_secpos = STAILQ_NEXT(dbg->dbgp_secpos, ds_next);

return (data);
}

void
dwarf_reset_section_bytes(Dwarf_P_Debug dbg)
{

assert(dbg != NULL);

dbg->dbgp_secpos = STAILQ_FIRST(&dbg->dbgp_seclist);
dbg->dbgp_drspos = STAILQ_FIRST(&dbg->dbgp_drslist);
}
