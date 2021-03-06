

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: dwarf_seterror.c 2075 2011-10-27 03:47:28Z jkoshy $");

#define _SET_FIELD(R, F, V) do { (R) = (F); (F) = (V); } while (0)





#define SET_FIELD(D, R, F) do { if (D) _SET_FIELD(R, (D)->dbg_##F, F); else _SET_FIELD(R, _libdwarf.F, F); } while (0)







Dwarf_Handler
dwarf_seterrhand(Dwarf_Debug dbg, Dwarf_Handler errhand)
{
Dwarf_Handler oldhandler;

SET_FIELD(dbg, oldhandler, errhand);

return (oldhandler);
}

Dwarf_Ptr
dwarf_seterrarg(Dwarf_Debug dbg, Dwarf_Ptr errarg)
{
Dwarf_Ptr oldarg;

SET_FIELD(dbg, oldarg, errarg);

return (oldarg);
}
