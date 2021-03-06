

























#include <sys/queue.h>
#include <errno.h>
#include <stdlib.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: pe_init.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

PE *
pe_init(int fd, PE_Cmd c, PE_Object o)
{
PE *pe;

if ((pe = calloc(1, sizeof(*pe))) == NULL) {
errno = ENOMEM;
return (NULL);
}
pe->pe_fd = fd;
pe->pe_cmd = c;
pe->pe_obj = o;
STAILQ_INIT(&pe->pe_scn);

switch (c) {
case PE_C_READ:
case PE_C_RDWR:
if (libpe_open_object(pe) < 0)
goto init_fail;
break;

case PE_C_WRITE:
if (o < PE_O_PE32 || o > PE_O_COFF) {
errno = EINVAL;
goto init_fail;
}
break;

default:
errno = EINVAL;
goto init_fail;
}

return (pe);

init_fail:
pe_finish(pe);
return (NULL);
}

void
pe_finish(PE *pe)
{

if (pe == NULL)
return;

libpe_release_object(pe);
}

PE_Object
pe_object(PE *pe)
{

if (pe == NULL) {
errno = EINVAL;
return (PE_O_UNKNOWN);
}

return (pe->pe_obj);
}
