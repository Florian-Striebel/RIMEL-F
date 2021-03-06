

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf_arange.c 3029 2014-04-21 23:26:02Z kaiwang27 $");

void
_dwarf_arange_cleanup(Dwarf_Debug dbg)
{
Dwarf_ArangeSet as, tas;
Dwarf_Arange ar, tar;

STAILQ_FOREACH_SAFE(as, &dbg->dbg_aslist, as_next, tas) {
STAILQ_FOREACH_SAFE(ar, &as->as_arlist, ar_next, tar) {
STAILQ_REMOVE(&as->as_arlist, ar, _Dwarf_Arange,
ar_next);
free(ar);
}
STAILQ_REMOVE(&dbg->dbg_aslist, as, _Dwarf_ArangeSet, as_next);
free(as);
}

if (dbg->dbg_arange_array)
free(dbg->dbg_arange_array);

dbg->dbg_arange_array = NULL;
dbg->dbg_arange_cnt = 0;
}

int
_dwarf_arange_init(Dwarf_Debug dbg, Dwarf_Error *error)
{
Dwarf_CU cu;
Dwarf_ArangeSet as;
Dwarf_Arange ar;
Dwarf_Section *ds;
uint64_t offset, dwarf_size, length, addr, range;
int i, ret;

ret = DW_DLE_NONE;

if ((ds = _dwarf_find_section(dbg, ".debug_aranges")) == NULL)
return (DW_DLE_NONE);

if (!dbg->dbg_info_loaded) {
ret = _dwarf_info_load(dbg, 1, 1, error);
if (ret != DW_DLE_NONE)
return (ret);
}

offset = 0;
while (offset < ds->ds_size) {

if ((as = malloc(sizeof(struct _Dwarf_ArangeSet))) == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
return (DW_DLE_MEMORY);
}
STAILQ_INIT(&as->as_arlist);
STAILQ_INSERT_TAIL(&dbg->dbg_aslist, as, as_next);


length = dbg->read(ds->ds_data, &offset, 4);
if (length == 0xffffffff) {
dwarf_size = 8;
length = dbg->read(ds->ds_data, &offset, 8);
} else
dwarf_size = 4;

as->as_length = length;
as->as_version = dbg->read(ds->ds_data, &offset, 2);
if (as->as_version != 2) {
DWARF_SET_ERROR(dbg, error, DW_DLE_VERSION_STAMP_ERROR);
ret = DW_DLE_VERSION_STAMP_ERROR;
goto fail_cleanup;
}

as->as_cu_offset = dbg->read(ds->ds_data, &offset, dwarf_size);
STAILQ_FOREACH(cu, &dbg->dbg_cu, cu_next) {
if (cu->cu_offset == as->as_cu_offset)
break;
}
if (cu == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARANGE_OFFSET_BAD);
ret = DW_DLE_ARANGE_OFFSET_BAD;
goto fail_cleanup;
}
as->as_cu = cu;

as->as_addrsz = dbg->read(ds->ds_data, &offset, 1);
as->as_segsz = dbg->read(ds->ds_data, &offset, 1);


offset = roundup(offset, 2 * as->as_addrsz);


while (offset < ds->ds_size) {
addr = dbg->read(ds->ds_data, &offset, as->as_addrsz);
range = dbg->read(ds->ds_data, &offset, as->as_addrsz);
if (addr == 0 && range == 0)
break;
if ((ar = calloc(1, sizeof(struct _Dwarf_Arange))) ==
NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
goto fail_cleanup;
}
ar->ar_as = as;
ar->ar_address = addr;
ar->ar_range = range;
STAILQ_INSERT_TAIL(&as->as_arlist, ar, ar_next);
dbg->dbg_arange_cnt++;
}
}


if (dbg->dbg_arange_cnt > 0) {
if ((dbg->dbg_arange_array = malloc(dbg->dbg_arange_cnt *
sizeof(Dwarf_Arange))) == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_MEMORY);
ret = DW_DLE_MEMORY;
goto fail_cleanup;
}

i = 0;
STAILQ_FOREACH(as, &dbg->dbg_aslist, as_next) {
STAILQ_FOREACH(ar, &as->as_arlist, ar_next)
dbg->dbg_arange_array[i++] = ar;
}
assert((Dwarf_Unsigned)i == dbg->dbg_arange_cnt);
}

return (DW_DLE_NONE);

fail_cleanup:

_dwarf_arange_cleanup(dbg);

return (ret);
}

int
_dwarf_arange_gen(Dwarf_P_Debug dbg, Dwarf_Error *error)
{
Dwarf_P_Section ds;
Dwarf_Rel_Section drs;
Dwarf_ArangeSet as;
Dwarf_Arange ar;
uint64_t offset;
int ret;

as = dbg->dbgp_as;
assert(as != NULL);
if (STAILQ_EMPTY(&as->as_arlist))
return (DW_DLE_NONE);

as->as_length = 0;
as->as_version = 2;
as->as_cu_offset = 0;
as->as_addrsz = dbg->dbg_pointer_size;
as->as_segsz = 0;


if ((ret = _dwarf_section_init(dbg, &ds, ".debug_aranges", 0, error)) !=
DW_DLE_NONE)
goto gen_fail0;


RCHECK(_dwarf_reloc_section_init(dbg, &drs, ds, error));


RCHECK(WRITE_VALUE(as->as_length, 4));
RCHECK(WRITE_VALUE(as->as_version, 2));
RCHECK(_dwarf_reloc_entry_add(dbg, drs, ds, dwarf_drt_data_reloc, 4,
ds->ds_size, 0, as->as_cu_offset, ".debug_info", error));
RCHECK(WRITE_VALUE(as->as_addrsz, 1));
RCHECK(WRITE_VALUE(as->as_segsz, 1));


offset = roundup(ds->ds_size, 2 * as->as_addrsz);
if (offset > ds->ds_size)
RCHECK(WRITE_PADDING(0, offset - ds->ds_size));


STAILQ_FOREACH(ar, &as->as_arlist, ar_next) {
RCHECK(_dwarf_reloc_entry_add(dbg, drs, ds,
dwarf_drt_data_reloc, dbg->dbg_pointer_size, ds->ds_size,
ar->ar_symndx, ar->ar_address, NULL, error));
if (ar->ar_esymndx > 0)
RCHECK(_dwarf_reloc_entry_add_pair(dbg, drs, ds,
dbg->dbg_pointer_size, ds->ds_size, ar->ar_symndx,
ar->ar_esymndx, ar->ar_address, ar->ar_eoff, error));
else
RCHECK(WRITE_VALUE(ar->ar_range, dbg->dbg_pointer_size));
}
RCHECK(WRITE_VALUE(0, dbg->dbg_pointer_size));
RCHECK(WRITE_VALUE(0, dbg->dbg_pointer_size));


as->as_length = ds->ds_size - 4;
offset = 0;
dbg->write(ds->ds_data, &offset, as->as_length, 4);


RCHECK(_dwarf_section_callback(dbg, ds, SHT_PROGBITS, 0, 0, 0, error));


RCHECK(_dwarf_reloc_section_finalize(dbg, drs, error));

return (DW_DLE_NONE);

gen_fail:
_dwarf_reloc_section_free(dbg, &drs);

gen_fail0:
_dwarf_section_free(dbg, &ds);

return (ret);
}

void
_dwarf_arange_pro_cleanup(Dwarf_P_Debug dbg)
{
Dwarf_ArangeSet as;
Dwarf_Arange ar, tar;

assert(dbg != NULL && dbg->dbg_mode == DW_DLC_WRITE);
if (dbg->dbgp_as == NULL)
return;

as = dbg->dbgp_as;
STAILQ_FOREACH_SAFE(ar, &as->as_arlist, ar_next, tar) {
STAILQ_REMOVE(&as->as_arlist, ar, _Dwarf_Arange, ar_next);
free(ar);
}
free(as);
dbg->dbgp_as = NULL;
}
