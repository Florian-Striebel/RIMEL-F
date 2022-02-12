

























#include <errno.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_cntl.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

int
pe_cntl(PE *pe, PE_Cmd cmd)
{

if (pe == NULL) {
errno = EINVAL;
return (-1);
}

switch (cmd) {
case PE_C_FDDONE:
pe->pe_flags |= LIBPE_F_FD_DONE;
break;

case PE_C_FDREAD:
if (pe->pe_cmd == PE_C_WRITE) {
errno = EACCES;
return (-1);
}
if (libpe_load_all_sections(pe) < 0)
return (-1);
break;

default:
errno = EINVAL;
return (-1);
}

return (0);
}
