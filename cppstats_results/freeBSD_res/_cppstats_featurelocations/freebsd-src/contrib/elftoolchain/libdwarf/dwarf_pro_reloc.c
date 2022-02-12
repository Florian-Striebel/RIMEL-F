

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_reloc.c 2074 2011-10-27 03:34:33Z jkoshy $");

int
dwarf_get_relocation_info_count(Dwarf_P_Debug dbg, Dwarf_Unsigned *reloc_cnt,
int *drd_buffer_version, Dwarf_Error *error)
{

if (dbg == NULL || reloc_cnt == NULL || drd_buffer_version == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if ((dbg->dbgp_flags & DW_DLC_SYMBOLIC_RELOCATIONS) == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

*reloc_cnt = dbg->dbgp_drscnt;
*drd_buffer_version = DWARF_DRD_BUFFER_VERSION;

return (DW_DLV_OK);
}

int
dwarf_get_relocation_info(Dwarf_P_Debug dbg, Dwarf_Signed *elf_section_index,
Dwarf_Signed *elf_section_link, Dwarf_Unsigned *reloc_entry_count,
Dwarf_Relocation_Data *reloc_buffer, Dwarf_Error *error)
{
Dwarf_Rel_Section drs;
Dwarf_Rel_Entry dre;
int i;

if (dbg == NULL || elf_section_index == NULL ||
elf_section_link == NULL || reloc_entry_count == NULL ||
reloc_buffer == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if ((dbg->dbgp_flags & DW_DLC_SYMBOLIC_RELOCATIONS) == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

if (dbg->dbgp_drscnt == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

if (dbg->dbgp_drspos == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

drs = dbg->dbgp_drspos;
assert(drs->drs_ds != NULL && drs->drs_ref != NULL);
assert(drs->drs_drecnt > 0);

*elf_section_index = drs->drs_ds->ds_ndx;
*elf_section_link = drs->drs_ref->ds_ndx;
*reloc_entry_count = drs->drs_drecnt;

if (drs->drs_drd == NULL) {
drs->drs_drd = calloc(*reloc_entry_count,
sizeof(struct Dwarf_Relocation_Data_s));
if (drs->drs_drd == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLV_ERROR);
}
for (i = 0, dre = STAILQ_FIRST(&drs->drs_dre);
(Dwarf_Unsigned) i < *reloc_entry_count && dre != NULL;
i++, dre = STAILQ_NEXT(dre, dre_next)) {
drs->drs_drd[i].drd_type = dre->dre_type;
drs->drs_drd[i].drd_length = dre->dre_length;
drs->drs_drd[i].drd_offset = dre->dre_offset;
drs->drs_drd[i].drd_symbol_index = dre->dre_symndx;
}
assert((Dwarf_Unsigned) i == *reloc_entry_count && dre == NULL);
}

*reloc_buffer = drs->drs_drd;

dbg->dbgp_drspos = STAILQ_NEXT(dbg->dbgp_drspos, drs_next);

return (DW_DLV_OK);
}
