

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf_str.c 2070 2011-10-27 03:05:32Z jkoshy $");

#define _INIT_DWARF_STRTAB_SIZE 1024

int
_dwarf_strtab_add(Dwarf_Debug dbg, char *string, uint64_t *off,
Dwarf_Error *error)
{
size_t len;

assert(dbg != NULL && string != NULL);

len = strlen(string) + 1;
while (dbg->dbg_strtab_size + len > dbg->dbg_strtab_cap) {
dbg->dbg_strtab_cap *= 2;
dbg->dbg_strtab = realloc(dbg->dbg_strtab,
(size_t) dbg->dbg_strtab_cap);
if (dbg->dbg_strtab == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
}

if (off != NULL)
*off = dbg->dbg_strtab_size;

strncpy(&dbg->dbg_strtab[dbg->dbg_strtab_size], string, len - 1);
dbg->dbg_strtab_size += len;
dbg->dbg_strtab[dbg->dbg_strtab_size - 1] = '\0';

return (DW_DLE_NONE);
}

char *
_dwarf_strtab_get_table(Dwarf_Debug dbg)
{

assert(dbg != NULL);

return (dbg->dbg_strtab);
}

int
_dwarf_strtab_init(Dwarf_Debug dbg, Dwarf_Error *error)
{
Dwarf_Section *ds;

assert(dbg != NULL);

if (dbg->dbg_mode == DW_DLC_READ || dbg->dbg_mode == DW_DLC_RDWR) {
ds = _dwarf_find_section(dbg, ".debug_str");
if (ds == NULL) {
dbg->dbg_strtab = NULL;
dbg->dbg_strtab_cap = dbg->dbg_strtab_size = 0;
return (DW_DLE_NONE);
}

dbg->dbg_strtab_cap = dbg->dbg_strtab_size = ds->ds_size;

if (dbg->dbg_mode == DW_DLC_RDWR) {
if ((dbg->dbg_strtab = malloc((size_t) ds->ds_size)) ==
NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
memcpy(dbg->dbg_strtab, ds->ds_data, ds->ds_size);
} else
dbg->dbg_strtab = (char *) ds->ds_data;
} else {


dbg->dbg_strtab_cap = _INIT_DWARF_STRTAB_SIZE;
dbg->dbg_strtab_size = 0;

if ((dbg->dbg_strtab = malloc((size_t) dbg->dbg_strtab_cap)) ==
NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}

dbg->dbg_strtab[0] = '\0';
}

return (DW_DLE_NONE);
}

void
_dwarf_strtab_cleanup(Dwarf_Debug dbg)
{

assert(dbg != NULL);

if (dbg->dbg_mode == DW_DLC_RDWR || dbg->dbg_mode == DW_DLC_WRITE)
free(dbg->dbg_strtab);
}

int
_dwarf_strtab_gen(Dwarf_P_Debug dbg, Dwarf_Error *error)
{
Dwarf_P_Section ds;
int ret;

assert(dbg != NULL);

if ((ret = _dwarf_section_init(dbg, &ds, ".debug_str", 0, error)) !=
DW_DLE_NONE)
return (ret);

if (dbg->dbg_strtab_size > ds->ds_cap) {
ds->ds_data = realloc(ds->ds_data,
(size_t) dbg->dbg_strtab_size);
if (ds->ds_data == NULL) {
_dwarf_section_free(dbg, &ds);
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
ds->ds_cap = dbg->dbg_strtab_size;
}

memcpy(ds->ds_data, dbg->dbg_strtab, dbg->dbg_strtab_size);
ds->ds_size = dbg->dbg_strtab_size;






ret = _dwarf_section_callback(dbg, ds, SHT_PROGBITS, 0, 0, 0, error);

return (ret);
}
