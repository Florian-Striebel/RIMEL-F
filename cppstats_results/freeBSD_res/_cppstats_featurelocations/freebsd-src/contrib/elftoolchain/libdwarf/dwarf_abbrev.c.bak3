

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_abbrev.c 2072 2011-10-27 03:26:49Z jkoshy $");

int
dwarf_get_abbrev(Dwarf_Debug dbg, Dwarf_Unsigned offset,
Dwarf_Abbrev *return_abbrev, Dwarf_Unsigned *length,
Dwarf_Unsigned *attr_count, Dwarf_Error *error)
{
Dwarf_Abbrev ab;
int ret;

if (dbg == NULL || return_abbrev == NULL || length == NULL ||
attr_count == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

ret = _dwarf_abbrev_parse(dbg, NULL, &offset, &ab, error);
if (ret != DW_DLE_NONE) {
if (ret == DW_DLE_NO_ENTRY) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
} else
return (DW_DLV_ERROR);
}

*return_abbrev = ab;
*length = ab->ab_length;
*attr_count = ab->ab_atnum;

return (DW_DLV_OK);
}

int
dwarf_get_abbrev_tag(Dwarf_Abbrev abbrev, Dwarf_Half *return_tag,
Dwarf_Error *error)
{

if (abbrev == NULL || return_tag == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*return_tag = (Dwarf_Half) abbrev->ab_tag;

return (DW_DLV_OK);
}

int
dwarf_get_abbrev_code(Dwarf_Abbrev abbrev, Dwarf_Unsigned *return_code,
Dwarf_Error *error)
{

if (abbrev == NULL || return_code == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*return_code = abbrev->ab_entry;

return (DW_DLV_OK);
}

int
dwarf_get_abbrev_children_flag(Dwarf_Abbrev abbrev, Dwarf_Signed *return_flag,
Dwarf_Error *error)
{

if (abbrev == NULL || return_flag == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

*return_flag = (Dwarf_Signed) abbrev->ab_children;

return (DW_DLV_OK);
}

int
dwarf_get_abbrev_entry(Dwarf_Abbrev abbrev, Dwarf_Signed ndx,
Dwarf_Half *attr_num, Dwarf_Signed *form, Dwarf_Off *offset,
Dwarf_Error *error)
{
Dwarf_AttrDef ad;
int i;

if (abbrev == NULL || attr_num == NULL || form == NULL ||
offset == NULL) {
DWARF_SET_ERROR(NULL, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (ndx < 0 || (uint64_t) ndx >= abbrev->ab_atnum) {
DWARF_SET_ERROR(NULL, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}

ad = STAILQ_FIRST(&abbrev->ab_attrdef);
for (i = 0; i < ndx && ad != NULL; i++)
ad = STAILQ_NEXT(ad, ad_next);

assert(ad != NULL);

*attr_num = ad->ad_attrib;
*form = ad->ad_form;
*offset = ad->ad_offset;

return (DW_DLV_OK);
}
