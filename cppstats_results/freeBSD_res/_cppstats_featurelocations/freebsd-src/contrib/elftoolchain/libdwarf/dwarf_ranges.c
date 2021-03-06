

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_ranges.c 3029 2014-04-21 23:26:02Z kaiwang27 $");

static int
_dwarf_get_ranges(Dwarf_Debug dbg, Dwarf_CU cu, Dwarf_Off off,
Dwarf_Ranges **ranges, Dwarf_Signed *ret_cnt, Dwarf_Unsigned *ret_byte_cnt,
Dwarf_Error *error)
{
Dwarf_Rangelist rl;
int ret;

assert(cu != NULL);
if (_dwarf_ranges_find(dbg, off, &rl) == DW_DLE_NO_ENTRY) {
ret = _dwarf_ranges_add(dbg, cu, off, &rl, error);
if (ret != DW_DLE_NONE)
return (DW_DLV_ERROR);
}

*ranges = rl->rl_rgarray;
*ret_cnt = rl->rl_rglen;

if (ret_byte_cnt != NULL)
*ret_byte_cnt = cu->cu_pointer_size * rl->rl_rglen * 2;

return (DW_DLV_OK);
}

int
dwarf_get_ranges(Dwarf_Debug dbg, Dwarf_Off offset, Dwarf_Ranges **ranges,
Dwarf_Signed *ret_cnt, Dwarf_Unsigned *ret_byte_cnt, Dwarf_Error *error)
{

if (dbg == NULL || ranges == NULL || ret_cnt == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (!dbg->dbg_info_loaded) {
if (_dwarf_info_load(dbg, 1, 1, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);
}

return (_dwarf_get_ranges(dbg, STAILQ_FIRST(&dbg->dbg_cu), offset,
ranges, ret_cnt, ret_byte_cnt, error));
}

int
dwarf_get_ranges_a(Dwarf_Debug dbg, Dwarf_Off offset, Dwarf_Die die,
Dwarf_Ranges **ranges, Dwarf_Signed *ret_cnt, Dwarf_Unsigned *ret_byte_cnt,
Dwarf_Error *error)
{

if (dbg == NULL || die == NULL || ranges == NULL || ret_cnt == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

return (_dwarf_get_ranges(dbg, die->die_cu, offset, ranges, ret_cnt,
ret_byte_cnt, error));
}
