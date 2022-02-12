


























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_init.c 2073 2011-10-27 03:30:47Z jkoshy $");

int
dwarf_elf_init(Elf *elf, int mode, Dwarf_Handler errhand, Dwarf_Ptr errarg,
Dwarf_Debug *ret_dbg, Dwarf_Error *error)
{
Dwarf_Debug dbg;
int ret;

if (elf == NULL || ret_dbg == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (mode != DW_DLC_READ) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (_dwarf_alloc(&dbg, mode, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);

if (_dwarf_elf_init(dbg, elf, error) != DW_DLE_NONE) {
free(dbg);
return (DW_DLV_ERROR);
}

if ((ret = _dwarf_init(dbg, 0, errhand, errarg, error)) !=
DW_DLE_NONE) {
_dwarf_elf_deinit(dbg);
free(dbg);
if (ret == DW_DLE_DEBUG_INFO_NULL)
return (DW_DLV_NO_ENTRY);
else
return (DW_DLV_ERROR);
}

*ret_dbg = dbg;

return (DW_DLV_OK);
}

int
dwarf_get_elf(Dwarf_Debug dbg, Elf **elf, Dwarf_Error *error)
{
Dwarf_Elf_Object *e;

if (dbg == NULL || elf == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

e = dbg->dbg_iface->object;
*elf = e->eo_elf;

return (DW_DLV_OK);
}

int
dwarf_init(int fd, int mode, Dwarf_Handler errhand, Dwarf_Ptr errarg,
Dwarf_Debug *ret_dbg, Dwarf_Error *error)
{
Dwarf_Debug dbg;
Elf *elf;
int ret;

if (fd < 0 || ret_dbg == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (mode != DW_DLC_READ) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (elf_version(EV_CURRENT) == EV_NONE) {
DWARF_SET_ELF_ERROR(NULL, error);
return (DW_DLV_ERROR);
}

if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
DWARF_SET_ELF_ERROR(NULL, error);
return (DW_DLV_ERROR);
}

if (_dwarf_alloc(&dbg, mode, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);

if (_dwarf_elf_init(dbg, elf, error) != DW_DLE_NONE) {
free(dbg);
return (DW_DLV_ERROR);
}

if ((ret = _dwarf_init(dbg, 0, errhand, errarg, error)) !=
DW_DLE_NONE) {
_dwarf_elf_deinit(dbg);
free(dbg);
if (ret == DW_DLE_DEBUG_INFO_NULL)
return (DW_DLV_NO_ENTRY);
else
return (DW_DLV_ERROR);
}

*ret_dbg = dbg;

return (DW_DLV_OK);
}

int
dwarf_object_init(Dwarf_Obj_Access_Interface *iface, Dwarf_Handler errhand,
Dwarf_Ptr errarg, Dwarf_Debug *ret_dbg, Dwarf_Error *error)
{
Dwarf_Debug dbg;

if (iface == NULL || ret_dbg == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (_dwarf_alloc(&dbg, DW_DLC_READ, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);

dbg->dbg_iface = iface;

if (_dwarf_init(dbg, 0, errhand, errarg, error) != DW_DLE_NONE) {
free(dbg);
return (DW_DLV_ERROR);
}

*ret_dbg = dbg;

return (DW_DLV_OK);
}
