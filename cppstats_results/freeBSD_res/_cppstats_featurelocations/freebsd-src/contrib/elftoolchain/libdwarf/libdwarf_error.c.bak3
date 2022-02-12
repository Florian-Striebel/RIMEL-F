

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf_error.c 2070 2011-10-27 03:05:32Z jkoshy $");

void
_dwarf_set_error(Dwarf_Debug dbg, Dwarf_Error *error, int errorcode,
int elferrorcode, const char *functionname, int linenumber)
{
Dwarf_Error de;

de.err_error = errorcode;
de.err_elferror = elferrorcode;
de.err_func = functionname;
de.err_line = linenumber;
de.err_msg[0] = '\0';








if (error)
*error = de;
else if (dbg && dbg->dbg_errhand)
dbg->dbg_errhand(de, dbg->dbg_errarg);
else if (_libdwarf.errhand)
_libdwarf.errhand(de, _libdwarf.errarg);


}
