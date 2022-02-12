

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_dealloc.c 2073 2011-10-27 03:30:47Z jkoshy $");

void
dwarf_dealloc(Dwarf_Debug dbg, Dwarf_Ptr p, Dwarf_Unsigned alloc_type)
{
Dwarf_Abbrev ab;
Dwarf_AttrDef ad, tad;
Dwarf_Attribute at, tat;
Dwarf_Die die;









(void) dbg;

if (alloc_type == DW_DLA_LIST || alloc_type == DW_DLA_FRAME_BLOCK ||
alloc_type == DW_DLA_LOC_BLOCK || alloc_type == DW_DLA_LOCDESC)
free(p);
else if (alloc_type == DW_DLA_ABBREV) {
ab = p;
STAILQ_FOREACH_SAFE(ad, &ab->ab_attrdef, ad_next, tad) {
STAILQ_REMOVE(&ab->ab_attrdef, ad, _Dwarf_AttrDef,
ad_next);
free(ad);
}
free(ab);
} else if (alloc_type == DW_DLA_DIE) {
die = p;
STAILQ_FOREACH_SAFE(at, &die->die_attr, at_next, tat) {
STAILQ_REMOVE(&die->die_attr, at,
_Dwarf_Attribute, at_next);
if (at->at_ld != NULL)
free(at->at_ld);
free(at);
}
if (die->die_attrarray)
free(die->die_attrarray);
free(die);
}
}

void
dwarf_srclines_dealloc(Dwarf_Debug dbg, Dwarf_Line *linebuf,
Dwarf_Signed count)
{







(void) dbg; (void) linebuf; (void) count;
}

void
dwarf_ranges_dealloc(Dwarf_Debug dbg, Dwarf_Ranges *ranges,
Dwarf_Signed range_count)
{







(void) dbg; (void) ranges; (void) range_count;
}

void
dwarf_fde_cie_list_dealloc(Dwarf_Debug dbg, Dwarf_Cie *cie_list,
Dwarf_Signed cie_count, Dwarf_Fde *fde_list, Dwarf_Signed fde_count)
{





(void) dbg;
(void) cie_list;
(void) cie_count;
(void) fde_list;
(void) fde_count;
}
