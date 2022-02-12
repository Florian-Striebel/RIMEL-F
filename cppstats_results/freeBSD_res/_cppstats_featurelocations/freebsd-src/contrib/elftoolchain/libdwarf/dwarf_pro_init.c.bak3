

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_pro_init.c 2074 2011-10-27 03:34:33Z jkoshy $");

Dwarf_P_Debug
dwarf_producer_init(Dwarf_Unsigned flags, Dwarf_Callback_Func func,
Dwarf_Handler errhand, Dwarf_Ptr errarg, Dwarf_Error *error)
{
Dwarf_P_Debug dbg;
int mode;

if (flags & DW_DLC_READ || flags & DW_DLC_RDWR) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (flags & DW_DLC_WRITE)
mode = DW_DLC_WRITE;
else {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (func == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_alloc(&dbg, DW_DLC_WRITE, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

dbg->dbg_mode = mode;

if (_dwarf_init(dbg, flags, errhand, errarg, error) != DW_DLE_NONE) {
free(dbg);
return (DW_DLV_BADADDR);
}

dbg->dbgp_func = func;

return (dbg);
}

Dwarf_P_Debug
dwarf_producer_init_b(Dwarf_Unsigned flags, Dwarf_Callback_Func_b func,
Dwarf_Handler errhand, Dwarf_Ptr errarg, Dwarf_Error *error)
{
Dwarf_P_Debug dbg;
int mode;

if (flags & DW_DLC_READ || flags & DW_DLC_RDWR) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (flags & DW_DLC_WRITE)
mode = DW_DLC_WRITE;
else {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (func == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_BADADDR);
}

if (_dwarf_alloc(&dbg, DW_DLC_WRITE, error) != DW_DLE_NONE)
return (DW_DLV_BADADDR);

dbg->dbg_mode = mode;

if (_dwarf_init(dbg, flags, errhand, errarg, error) != DW_DLE_NONE) {
free(dbg);
return (DW_DLV_BADADDR);
}

dbg->dbgp_func_b = func;

return (dbg);
}

int
dwarf_producer_set_isa(Dwarf_P_Debug dbg, enum Dwarf_ISA isa,
Dwarf_Error *error)
{

if (dbg == NULL || isa >= DW_ISA_MAX) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

dbg->dbgp_isa = isa;

return (DW_DLV_OK);
}
