

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_arange.c 2072 2011-10-27 03:26:49Z jkoshy $");

int
dwarf_get_aranges(Dwarf_Debug dbg, Dwarf_Arange **arlist,
Dwarf_Signed *ret_arange_cnt, Dwarf_Error *error)
{

if (dbg == NULL || arlist == NULL || ret_arange_cnt == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (dbg->dbg_arange_cnt == 0) {
if (_dwarf_arange_init(dbg, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);
if (dbg->dbg_arange_cnt == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}
}

assert(dbg->dbg_arange_array != NULL);

*arlist = dbg->dbg_arange_array;
*ret_arange_cnt = dbg->dbg_arange_cnt;

return (DW_DLV_OK);
}

int
dwarf_get_arange(Dwarf_Arange *arlist, Dwarf_Unsigned arange_cnt,
Dwarf_Addr addr, Dwarf_Arange *ret_arange, Dwarf_Error *error)
{
Dwarf_Arange ar;
Dwarf_Debug dbg;
int i;

if (arlist == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

dbg = (*arlist)->ar_as->as_cu->cu_dbg;

if (ret_arange == NULL || arange_cnt == 0) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

for (i = 0; (Dwarf_Unsigned)i < arange_cnt; i++) {
ar = arlist[i];
if (addr >= ar->ar_address && addr < ar->ar_address +
ar->ar_range) {
*ret_arange = ar;
return (DW_DLV_OK);
}
}

DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);

return (DW_DLV_NO_ENTRY);
}

int
dwarf_get_cu_die_offset(Dwarf_Arange ar, Dwarf_Off *ret_offset,
Dwarf_Error *error)
{
Dwarf_CU cu;
Dwarf_ArangeSet as;

if (ar == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

as = ar->ar_as;
assert(as != NULL);
cu = as->as_cu;
assert(cu != NULL);

if (ret_offset == NULL) {
DWARF_SET_ERROR(cu->cu_dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*ret_offset = cu->cu_1st_offset;

return (DW_DLV_OK);
}

int
dwarf_get_arange_cu_header_offset(Dwarf_Arange ar, Dwarf_Off *ret_offset,
Dwarf_Error *error)
{
Dwarf_ArangeSet as;

if (ar == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

as = ar->ar_as;
assert(as != NULL);

if (ret_offset == NULL) {
DWARF_SET_ERROR(as->as_cu->cu_dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*ret_offset = as->as_cu_offset;

return (DW_DLV_OK);
}

int
dwarf_get_arange_info(Dwarf_Arange ar, Dwarf_Addr *start,
Dwarf_Unsigned *length, Dwarf_Off *cu_die_offset, Dwarf_Error *error)
{
Dwarf_CU cu;
Dwarf_ArangeSet as;

if (ar == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

as = ar->ar_as;
assert(as != NULL);
cu = as->as_cu;
assert(cu != NULL);

if (start == NULL || length == NULL ||
cu_die_offset == NULL) {
DWARF_SET_ERROR(cu->cu_dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*start = ar->ar_address;
*length = ar->ar_range;
*cu_die_offset = cu->cu_1st_offset;

return (DW_DLV_OK);
}
