

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_macinfo.c 2074 2011-10-27 03:34:33Z jkoshy $");

char *
dwarf_find_macro_value_start(char *macro_string)
{
char *p;

if (macro_string == NULL)
return (NULL);

p = macro_string;
while (*p != '\0' && *p != ' ')
p++;
if (*p == ' ')
p++;

return (p);
}

int
dwarf_get_macro_details(Dwarf_Debug dbg, Dwarf_Off offset,
Dwarf_Unsigned max_count, Dwarf_Signed *entry_cnt,
Dwarf_Macro_Details **details, Dwarf_Error *error)
{
Dwarf_MacroSet ms;
Dwarf_Unsigned cnt;
int i;

if (dbg == NULL || entry_cnt == NULL || details == NULL) {
DWARF_SET_ERROR(dbg, error, DW_DLE_ARGUMENT);
return (DW_DLV_ERROR);
}

if (STAILQ_EMPTY(&dbg->dbg_mslist)) {
if (_dwarf_macinfo_init(dbg, error) != DW_DLE_NONE)
return (DW_DLV_ERROR);
if (STAILQ_EMPTY(&dbg->dbg_mslist)) {
DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);
return (DW_DLV_NO_ENTRY);
}
}

STAILQ_FOREACH(ms, &dbg->dbg_mslist, ms_next) {
for (i = 0; (Dwarf_Unsigned) i < ms->ms_cnt; i++)
if (ms->ms_mdlist[i].dmd_offset == offset) {
cnt = ms->ms_cnt - i;
if (max_count != 0 && cnt > max_count)
cnt = max_count;

*details = &ms->ms_mdlist[i];
*entry_cnt = cnt;

return (DW_DLV_OK);
}
}

DWARF_SET_ERROR(dbg, error, DW_DLE_NO_ENTRY);

return (DW_DLV_NO_ENTRY);
}
