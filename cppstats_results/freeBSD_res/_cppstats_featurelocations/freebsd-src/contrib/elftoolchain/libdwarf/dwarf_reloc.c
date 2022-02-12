

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_reloc.c 3161 2015-02-15 21:43:36Z emaste $");

int
dwarf_set_reloc_application(int apply)
{
int oldapply;

oldapply = _libdwarf.applyreloc;
_libdwarf.applyreloc = apply;

return (oldapply);
}
